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

enum FileListDirFlags : uint8_t {
  IncludeSymlinks = 1, // Should symbolic links be included in directory listings.
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
#else //!_WIN32
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
    *pErr = getFilePlatformError();
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
    *pErr = getFilePlatformError();
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

inline auto getFileModTimeSinceMicro(PlatformError* pErr, StringRef* path) -> int64_t {
#if defined(_WIN32)

  WIN32_FILE_ATTRIBUTE_DATA data;
  const bool success = ::GetFileAttributesExA(path->getCharDataPtr(), GetFileExInfoStandard, &data);

  if (!success) {
    *pErr = getFilePlatformError();
    return -1L;
  }

  return winFileTimeToMicroSinceEpoch(data.ftLastWriteTime);

#else // !_WIN32

  struct stat statResult;
  if (::stat(path->getCharDataPtr(), &statResult) != 0) {
    *pErr = getFilePlatformError();
    return -1L;
  }
#if defined(__APPLE__)
  return statResult.st_mtimespec.tv_sec * 1'000'000L + statResult.st_mtimespec.tv_nsec / 1'000;
#else // !__APPLE__
  return statResult.st_mtim.tv_sec * 1'000'000L + statResult.st_mtim.tv_nsec / 1'000;
#endif // !__APPLE__

#endif // !_WIN32
}

inline auto getFileSize(PlatformError* pErr, StringRef* path) -> int64_t {
#if defined(_WIN32)

  WIN32_FILE_ATTRIBUTE_DATA data;
  const bool success = ::GetFileAttributesExA(path->getCharDataPtr(), GetFileExInfoStandard, &data);

  if (!success) {
    *pErr = getFilePlatformError();
    return -1L;
  }
  LARGE_INTEGER fileSize;
  fileSize.LowPart  = data.nFileSizeLow;
  fileSize.HighPart = data.nFileSizeHigh;
  return static_cast<int64_t>(fileSize.QuadPart);

#else // !_WIN32

  struct stat statResult;
  if (::stat(path->getCharDataPtr(), &statResult) != 0) {
    *pErr = getFilePlatformError();
    return -1L;
  }
  return static_cast<int64_t>(statResult.st_size);

#endif // !_WIN32
}

inline auto createFileDir(PlatformError* pErr, StringRef* path) -> bool {
#if defined(_WIN32)

  const bool success = ::CreateDirectoryA(path->getCharDataPtr(), nullptr);
  if (!success) {
    *pErr = getFilePlatformError();
  }
  return success;

#else // !_WIN32

  // RWX for owner and group, RX for others.
  const int newDirPerms =
      S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH;

  const int res = ::mkdir(path->getCharDataPtr(), newDirPerms);
  if (res != 0) {
    *pErr = getFilePlatformError();
  }
  return res == 0;

#endif // !_WIN32
}

inline auto removeFile(PlatformError* pErr, StringRef* path) -> bool {
#if defined(_WIN32)

  const bool success = ::DeleteFileA(path->getCharDataPtr());
  if (!success) {
    *pErr = getFilePlatformError();
  }
  return success;

#else // !_WIN32

  const int res = ::unlink(path->getCharDataPtr());
  if (res != 0) {
    *pErr = getFilePlatformError();
  }
  return res == 0;

#endif // !_WIN32
}

inline auto removeFileDir(PlatformError* pErr, StringRef* path) -> bool {
#if defined(_WIN32)

  const bool success = ::RemoveDirectoryA(path->getCharDataPtr());
  if (!success) {
    *pErr = getFilePlatformError();
  }
  return success;

#else // !_WIN32

  const int res = ::rmdir(path->getCharDataPtr());
  if (res != 0) {
    *pErr = getFilePlatformError();
  }
  return res == 0;

#endif // !_WIN32
}

inline auto renameFile(PlatformError* pErr, StringRef* oldPath, StringRef* newPath) -> bool {
#if defined(_WIN32)

  const bool success = ::MoveFileExA(
      oldPath->getCharDataPtr(), newPath->getCharDataPtr(), MOVEFILE_REPLACE_EXISTING);
  if (!success) {
    *pErr = getFilePlatformError();
  }
  return success;

#else // !_WIN32

  const int res = ::rename(oldPath->getCharDataPtr(), newPath->getCharDataPtr());
  if (res != 0) {
    *pErr = getFilePlatformError();
  }
  return res == 0;

#endif // !_WIN32
}

inline auto
fileListDir(RefAllocator* refAlloc, PlatformError* pErr, StringRef* path, FileListDirFlags flags)
    -> StringRef* {

  StringRef* buffer = nullptr;
  char* writeHead   = nullptr;
  char* bufferEnd   = nullptr;

#if defined(_WIN32)

  if (path->getSize() > MAX_PATH) {
    *pErr = PlatformError::FilePathTooLong;
    return refAlloc->allocStr(0);
  }

  // Append '/*' to the input path to turn it into a filter for FindFirstFile.
  // Info: https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfileexa
  char inputPathBuffer[MAX_PATH + 2];
  ::memcpy(inputPathBuffer, path->getCharDataPtr(), path->getSize());
  ::memcpy(inputPathBuffer + path->getSize(), "/*", 3); // 3 to copy the null-terminator also.

  WIN32_FIND_DATA findData;
  HANDLE searchHandle = ::FindFirstFileExA(
      inputPathBuffer, FindExInfoBasic, &findData, FindExSearchNameMatch, nullptr, 0);
  if (searchHandle == INVALID_HANDLE_VALUE) {
    *pErr = getFilePlatformError();
    return refAlloc->allocStr(0);
  }

  do {
    const bool isSymlink = (findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
        (findData.dwReserved0 == IO_REPARSE_TAG_SYMLINK);
    if ((flags & IncludeSymlinks) == 0 && isSymlink) {
      continue;
    }

    const size_t nameLength = ::strlen(findData.cFileName);
    if (findData.cFileName[0] == '.' &&
        (nameLength == 1 || (nameLength == 2 && findData.cFileName[1] == '.'))) {
      // Skip the '.' and '..' entries.
      continue;
    }

    if (writeHead + nameLength + 1 >= bufferEnd) { // Note: +1 for the seperating newline.
      StringRef* newBuffer = refAlloc->allocStr(buffer ? buffer->getSize() * 2 : (MAX_PATH * 2));
      if (unlikely(newBuffer == nullptr)) {
        ::FindClose(searchHandle);
        return nullptr;
      }
      char* oldWriteHead = writeHead;
      writeHead          = newBuffer->getCharDataPtr();
      bufferEnd          = writeHead + newBuffer->getSize();
      if (buffer) { // Copy the contents of the old buffer to the new buffer.
        ::memcpy(writeHead, buffer->getCharDataPtr(), buffer->getSize());
        writeHead += oldWriteHead - buffer->getCharDataPtr();
      }
      buffer = newBuffer;
    }
    ::memcpy(writeHead, findData.cFileName, nameLength);
    writeHead += nameLength;
    *writeHead++ = '\n';
  } while (::FindNextFileA(searchHandle, &findData));

  if (!::FindClose(searchHandle)) {
    *pErr = PlatformError::FileUnknownError;
    return refAlloc->allocStr(0);
  }

#else // !_WIN32

  DIR* dir = ::opendir(path->getCharDataPtr());
  if (!dir) {
    *pErr = getFilePlatformError();
    return refAlloc->allocStr(0);
  }

  while (struct dirent* dirEnt = ::readdir(dir)) {
    if ((flags & IncludeSymlinks) == 0 && dirEnt->d_type == DT_LNK) {
      continue;
    }

    const size_t nameLength = ::strlen(dirEnt->d_name);
    if (dirEnt->d_name[0] == '.' &&
        (nameLength == 1 || (nameLength == 2 && dirEnt->d_name[1] == '.'))) {
      // Skip the '.' and '..' entries.
      continue;
    }

    if (writeHead + nameLength + 1 >= bufferEnd) { // Note: +1 for the seperating newline.
      StringRef* newBuffer = refAlloc->allocStr(buffer ? buffer->getSize() * 2 : (NAME_MAX * 2));
      if (unlikely(newBuffer == nullptr)) {
        ::closedir(dir);
        return nullptr;
      }
      char* oldWriteHead = writeHead;
      writeHead = newBuffer->getCharDataPtr();
      bufferEnd = writeHead + newBuffer->getSize();
      if (buffer) { // Copy the contents of the old buffer to the new buffer.
        ::memcpy(writeHead, buffer->getCharDataPtr(), buffer->getSize());
        writeHead += oldWriteHead - buffer->getCharDataPtr();
      }
      buffer = newBuffer;
    }
    ::memcpy(writeHead, dirEnt->d_name, nameLength);
    writeHead += nameLength;
    *writeHead++ = '\n';
  }

  if (::closedir(dir)) {
    *pErr = PlatformError::FileUnknownError;
    return refAlloc->allocStr(0);
  }
#endif // !_WIN32

  *pErr = PlatformError::None;
  if (buffer) {
    buffer->updateSize(writeHead - buffer->getCharDataPtr());
    return buffer;
  }
  return refAlloc->allocStr(0);
}

inline auto getFilePlatformError() noexcept -> PlatformError {
#if defined(_WIN32)

  switch (::GetLastError()) {
  case ERROR_ACCESS_DENIED:
    return PlatformError::FileNoAccess;
  case ERROR_SHARING_VIOLATION:
    return PlatformError::FileLocked;
  case ERROR_FILE_NOT_FOUND:
  case ERROR_PATH_NOT_FOUND:
  case ERROR_INVALID_DRIVE:
    return PlatformError::FileNotFound;
  case ERROR_BUFFER_OVERFLOW:
    return PlatformError::FilePathTooLong;
  case ERROR_DISK_FULL:
    return PlatformError::FileDiskFull;
  case ERROR_DIRECTORY:
    return PlatformError::FileIsNotDirectory;
  case ERROR_DIR_NOT_EMPTY:
    return PlatformError::FileDirectoryNotEmpty;
  case ERROR_ALREADY_EXISTS:
    return PlatformError::FileAlreadyExists;
  case ERROR_INVALID_NAME:
  case ERROR_BAD_PATHNAME:
    return PlatformError::FileInvalidFileName;
  case ERROR_TOO_MANY_OPEN_FILES:
    return PlatformError::FileTooManyOpenFiles;
  }

#else // !_WIN32

  switch (errno) {
  case EACCES:
  case EPERM:
  case EROFS:
    return PlatformError::FileNoAccess;
  case ETXTBSY:
    return PlatformError::FileLocked;
  case EDQUOT:
  case ENOSPC:
    return PlatformError::FileDiskFull;
  case ENOENT:
    return PlatformError::FileNotFound;
  case EISDIR:
    return PlatformError::FileIsDirectory;
  case ENOTDIR:
    return PlatformError::FileIsNotDirectory;
  case ENAMETOOLONG:
    return PlatformError::FilePathTooLong;
  case EEXIST:
    return PlatformError::FileAlreadyExists;
  case ENOTEMPTY:
    return PlatformError::FileDirectoryNotEmpty;
  case EINVAL:
    return PlatformError::FileInvalidFileName;
  case EMFILE:
    return PlatformError::FileTooManyOpen;
  }

#endif // !_WIN32
  return PlatformError::FileUnknownError;
}

} // namespace vm::internal
