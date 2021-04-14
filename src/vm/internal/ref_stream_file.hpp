#pragma once
#include "gsl.hpp"
#include "internal/os_include.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "internal/stream_opts.hpp"
#include "intrinsics.hpp"
#include "vm/file.hpp"

namespace vm::internal {

enum class FileType : uint8_t {
  None      = 0,
  Regular   = 1,
  Directory = 2,
  Socket    = 3,
  Character = 4,
  Unknown   = 5,
};

enum class FileStreamMode : uint8_t {
  Create = 0,
  Open   = 1,
  Append = 2,
};

enum FileStreamFlags : uint8_t {
  AutoRemoveFile = 1, // File is deleted when the reference to it is freed.
};

auto getFilePlatformError() noexcept -> PlatformError;

// File implementation of the 'stream' interface.
// Note: To avoid needing a vtable there is no abstract 'Stream' class but instead there are wrapper
// functions that dispatch based on the 'RefKind' (see stream_utilities.hpp).
class FileStreamRef final : public Ref {
  friend class RefAllocator;

public:
  FileStreamRef(const FileStreamRef& rhs) = delete;
  FileStreamRef(FileStreamRef&& rhs)      = delete;
  ~FileStreamRef() noexcept {
    if (fileIsValid(m_fileHandle)) {
      fileClose(m_fileHandle);
    }
    if ((m_flags & AutoRemoveFile) == AutoRemoveFile) {
#if defined(_WIN32)
      ::DeleteFileA(m_filePath);
#else  //!_WIN32
      ::unlink(m_filePath);
#endif //!_WIN32
    }
    delete[] m_filePath;
  }

  auto operator=(const FileStreamRef& rhs) -> FileStreamRef& = delete;
  auto operator=(FileStreamRef&& rhs) -> FileStreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::StreamFile; }

  [[nodiscard]] auto isValid() noexcept -> bool { return fileIsValid(m_fileHandle); }

  auto readString(ExecutorHandle* execHandle, PlatformError* pErr, StringRef* str) noexcept
      -> bool {
    if (unlikely(str->getSize() == 0)) {
      return true;
    }

    execHandle->setState(ExecState::Paused);

    const int bytesRead = fileRead(m_fileHandle, str->getCharDataPtr(), str->getSize());

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false; // Aborted.
    }

    if (bytesRead < 0) {
      *pErr = getFilePlatformError();
      str->updateSize(0);
      return false;
    }
    if (bytesRead == 0) {
      *pErr = PlatformError::StreamNoDataAvailable;
      str->updateSize(0);
      return false;
    }
    str->updateSize(bytesRead);
    return bytesRead > 0;
  }

  auto writeString(ExecutorHandle* execHandle, PlatformError* pErr, StringRef* str) noexcept
      -> bool {

    if (unlikely(str->getSize() == 0)) {
      return true;
    }

    execHandle->setState(ExecState::Paused);

    const int bytesWritten = fileWrite(m_fileHandle, str->getCharDataPtr(), str->getSize());

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    if (bytesWritten != static_cast<int>(str->getSize())) {
      *pErr = getFilePlatformError();
      return false;
    }
    return true;
  }

  auto setOpts(PlatformError* pErr, StreamOpts /*unused*/) noexcept -> bool {
    // TODO: Support non-blocking file handles.
    *pErr = PlatformError::StreamOptionsNotSupported;
    return false;
  }

  auto unsetOpts(PlatformError* pErr, StreamOpts /*unused*/) noexcept -> bool {
    // TODO: Support non-blocking file handles.
    *pErr = PlatformError::StreamOptionsNotSupported;
    return false;
  }

private:
  FileStreamFlags m_flags;
  FileHandle m_fileHandle;
  gsl::owner<char*> m_filePath;

  inline explicit FileStreamRef(
      FileHandle fileHandle, gsl::owner<char*> filePath, FileStreamFlags flags) noexcept :
      Ref{getKind()}, m_flags{flags}, m_fileHandle{fileHandle}, m_filePath{filePath} {}
};

inline auto openFileStream(
    RefAllocator* alloc, PlatformError* pErr, StringRef* path, FileStreamMode m, FileStreamFlags f)
    -> FileStreamRef* {

  // Make copy of the file-path string to store in the file-stream.
  auto filePath = new gsl::owner<char>[path->getSize() + 1];   // +1 for null-terminator.
  ::memcpy(filePath, path->getDataPtr(), path->getSize() + 1); // +1 for null-terminator.

#if defined(_WIN32)
  DWORD desiredAccess       = 0;
  DWORD shareMode           = FILE_SHARE_READ | FILE_SHARE_WRITE; // TODO: Should we limit this?
  DWORD creationDisposition = 0;
  DWORD flags               = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_POSIX_SEMANTICS;
  switch (m) {
  default:
  case FileStreamMode::Open:
    desiredAccess |= GENERIC_READ | GENERIC_WRITE;
    creationDisposition = OPEN_EXISTING;
    break;
  case FileStreamMode::Append:
    desiredAccess |= FILE_APPEND_DATA;
    creationDisposition = OPEN_EXISTING;
    break;
  case FileStreamMode::Create:
    desiredAccess |= GENERIC_READ | GENERIC_WRITE;
    creationDisposition = CREATE_ALWAYS;
    break;
  }

  const FileHandle file = ::CreateFileA(
      filePath, desiredAccess, shareMode, nullptr, creationDisposition, flags, nullptr);
  if (!fileIsValid(file)) {
    switch (::GetLastError()) {
    case ERROR_ACCESS_DENIED:
      *pErr = PlatformError::FileNoAccess;
      break;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_DRIVE:
      *pErr = PlatformError::FileNotFound;
      break;
    case ERROR_INVALID_NAME:
    case ERROR_BAD_PATHNAME:
      *pErr = PlatformError::FileInvalidFileName;
      break;
    case ERROR_BUFFER_OVERFLOW:
      *pErr = PlatformError::FilePathTooLong;
      break;
    case ERROR_SHARING_VIOLATION:
      *pErr = PlatformError::FileLocked;
      break;
    default:
      *pErr = PlatformError::FileUnknownError;
      break;
    }
  }
  return alloc->allocPlain<FileStreamRef>(file, filePath, f);

#else //!_WIN32

  int flags = O_RDWR | O_NOCTTY;
  switch (m) {
  default:
  case FileStreamMode::Open:
    break;
  case FileStreamMode::Append:
    flags |= O_APPEND;
    break;
  case FileStreamMode::Create:
    flags |= O_CREAT | O_TRUNC;
    break;
  }
  const int newFilePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // RW for owner, and R for others.
  const FileHandle file = ::open(filePath, flags, newFilePerms);
  if (!fileIsValid(file)) {
    switch (errno) {
    case EPERM:
    case EACCES:
      *pErr = PlatformError::FileNoAccess;
      break;
    case EINVAL:
      *pErr = PlatformError::FileInvalidFileName;
      break;
    case ENAMETOOLONG:
      *pErr = PlatformError::FilePathTooLong;
      break;
    case ENOENT:
    case ENOTDIR:
      *pErr = PlatformError::FileNotFound;
      break;
    default:
      *pErr = PlatformError::FileUnknownError;
      break;
    }
  }
  return alloc->allocPlain<FileStreamRef>(file, filePath, f);

#endif
}

inline auto getFileType(StringRef* path) -> FileType {
#if defined(_WIN32)

  const DWORD attributes = ::GetFileAttributesA(path->getCharDataPtr());
  if (attributes == INVALID_FILE_ATTRIBUTES) {
    return FileType::None;
  }
  if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
    return FileType::Directory;
  }
  return FileType::Regular;

#else // !_WIN32

  struct stat statResult;
  if (::stat(path->getCharDataPtr(), &statResult) != 0) {
    return FileType::None;
  }
  if (S_ISREG(statResult.st_mode)) {
    return FileType::Regular;
  }
  if (S_ISDIR(statResult.st_mode)) {
    return FileType::Directory;
  }
  if (S_ISSOCK(statResult.st_mode)) {
    return FileType::Socket;
  }
  if (S_ISCHR(statResult.st_mode)) {
    return FileType::Character;
  }
  return FileType::Unknown;

#endif // !_WIN32
}

inline auto createFileDir(PlatformError* pErr, StringRef* path) -> bool {
#if defined(_WIN32)

  const bool success = ::CreateDirectoryA(path->getCharDataPtr(), nullptr);
  if (!success) {
    switch (::GetLastError()) {
    case ERROR_ACCESS_DENIED:
      *pErr = PlatformError::FileNoAccess;
      break;
    case ERROR_SHARING_VIOLATION:
      *pErr = PlatformError::FileLocked;
      break;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_DRIVE:
      *pErr = PlatformError::FileNotFound;
      break;
    case ERROR_ALREADY_EXISTS:
      *pErr = PlatformError::FileAlreadyExists;
      break;
    default:
      *pErr = PlatformError::FileUnknownError;
      break;
    }
  }
  return success;

#else // !_WIN32

  const int newDirPerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH; // RW for owner, and R for others.
  const int res = ::mkdir(path->getCharDataPtr(), newDirPerms);
  if (res != 0) {
    switch (errno) {
    case EACCES:
    case EPERM:
    case EROFS:
      *pErr = PlatformError::FileNoAccess;
      break;
    case EDQUOT:
    case ENOSPC:
      *pErr = PlatformError::FileDiskFull;
      break;
    case EEXIST:
      *pErr = PlatformError::FileAlreadyExists;
      break;
    case ENOTDIR:
      *pErr = PlatformError::FileIsNotDirectory;
      break;
    case ENOENT:
      *pErr = PlatformError::FileNotFound;
      break;
    case ENAMETOOLONG:
      *pErr = PlatformError::FilePathTooLong;
      break;
    default:
      *pErr = PlatformError::FileUnknownError;
      break;
    }
  }
  return res == 0;

#endif // !_WIN32
}

inline auto removeFile(PlatformError* pErr, StringRef* path) -> bool {
#if defined(_WIN32)

  const bool success = ::DeleteFileA(path->getCharDataPtr());
  if (!success) {
    switch (::GetLastError()) {
    case ERROR_ACCESS_DENIED:
      *pErr = PlatformError::FileNoAccess;
      break;
    case ERROR_SHARING_VIOLATION:
      *pErr = PlatformError::FileLocked;
      break;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_DRIVE:
      *pErr = PlatformError::FileNotFound;
      break;
    default:
      *pErr = PlatformError::FileUnknownError;
      break;
    }
  }
  return success;

#else // !_WIN32

  const int res = ::unlink(path->getCharDataPtr());
  if (res != 0) {
    switch (errno) {
    case EACCES:
    case EPERM:
    case EROFS:
      *pErr = PlatformError::FileNoAccess;
      break;
    case ENOENT:
      *pErr = PlatformError::FileNotFound;
      break;
    case EISDIR:
      *pErr = PlatformError::FileIsDirectory;
      break;
    default:
      *pErr = PlatformError::FileUnknownError;
      break;
    }
  }
  return res == 0;

#endif // !_WIN32
}

inline auto removeFileDir(PlatformError* pErr, StringRef* path) -> bool {
#if defined(_WIN32)

  const bool success = ::RemoveDirectoryA(path->getCharDataPtr());
  if (!success) {
    switch (::GetLastError()) {
    case ERROR_ACCESS_DENIED:
      *pErr = PlatformError::FileNoAccess;
      break;
    case ERROR_SHARING_VIOLATION:
      *pErr = PlatformError::FileLocked;
      break;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_DRIVE:
      *pErr = PlatformError::FileNotFound;
      break;
    case ERROR_DIRECTORY:
      *pErr = PlatformError::FileIsNotDirectory;
      break;
    case ERROR_DIR_NOT_EMPTY:
      *pErr = PlatformError::FileDirectoryNotEmpty;
      break;
    default:
      *pErr = PlatformError::FileUnknownError;
      break;
    }
  }
  return success;

#else // !_WIN32

  const int res = ::rmdir(path->getCharDataPtr());
  if (res != 0) {
    switch (errno) {
    case EACCES:
    case EPERM:
    case EROFS:
      *pErr = PlatformError::FileNoAccess;
      break;
    case ENOENT:
      *pErr = PlatformError::FileNotFound;
      break;
    case ENOTDIR:
      *pErr = PlatformError::FileIsNotDirectory;
      break;
    case EEXIST:
    case ENOTEMPTY:
      *pErr = PlatformError::FileDirectoryNotEmpty;
      break;
    default:
      *pErr = PlatformError::FileUnknownError;
      break;
    }
  }
  return res == 0;

#endif // !_WIN32
}

inline auto renameFile(PlatformError* pErr, StringRef* oldPath, StringRef* newPath) -> bool {
#if defined(_WIN32)

  const bool success = ::MoveFileExA(
      oldPath->getCharDataPtr(), newPath->getCharDataPtr(), MOVEFILE_REPLACE_EXISTING);
  if (!success) {
    switch (::GetLastError()) {
    case ERROR_ACCESS_DENIED:
      *pErr = PlatformError::FileNoAccess;
      break;
    case ERROR_SHARING_VIOLATION:
      *pErr = PlatformError::FileLocked;
      break;
    case ERROR_FILE_NOT_FOUND:
    case ERROR_PATH_NOT_FOUND:
    case ERROR_INVALID_DRIVE:
      *pErr = PlatformError::FileNotFound;
      break;
    case ERROR_BUFFER_OVERFLOW:
      *pErr = PlatformError::FilePathTooLong;
      break;
    default:
      *pErr = PlatformError::FileUnknownError;
      break;
    }
  }
  return success;

#else // !_WIN32

  const int res = ::rename(oldPath->getCharDataPtr(), newPath->getCharDataPtr());
  if (res != 0) {
    switch (errno) {
    case EACCES:
    case EPERM:
    case EROFS:
      *pErr = PlatformError::FileNoAccess;
      break;
    case EDQUOT:
    case ENOSPC:
      *pErr = PlatformError::FileDiskFull;
      break;
    case ENOENT:
      *pErr = PlatformError::FileNotFound;
      break;
    case EISDIR:
      *pErr = PlatformError::FileIsDirectory;
      break;
    case ENOTDIR:
      *pErr = PlatformError::FileIsNotDirectory;
      break;
    case ENAMETOOLONG:
      *pErr = PlatformError::FilePathTooLong;
      break;
    case EEXIST:
    case ENOTEMPTY:
      *pErr = PlatformError::FileDirectoryNotEmpty;
      break;
    default:
      *pErr = PlatformError::FileUnknownError;
      break;
    }
  }
  return res == 0;

#endif // !_WIN32
}

inline auto getFilePlatformError() noexcept -> PlatformError {
#if defined(_WIN32)

  switch (::GetLastError()) {
  case ERROR_DISK_FULL:
    return PlatformError::FileDiskFull;
  }

#else // !_WIN32

  switch (errno) {
  case EDQUOT:
    return PlatformError::FileDiskFull;
  }

#endif // !_WIN32
  return PlatformError::FileUnknownError;
}

} // namespace vm::internal
