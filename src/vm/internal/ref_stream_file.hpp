#pragma once
#include "gsl.hpp"
#include "internal/fd_utilities.hpp"
#include "internal/os_include.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "intrinsics.hpp"

#if defined(_WIN32)

#define OS_FILE HANDLE
#define OS_INVALID_FILE INVALID_HANDLE_VALUE

#else // !_WIN32

#define OS_FILE int
#define OS_INVALID_FILE -1

#endif // !_WIN32

namespace vm::internal {

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
    if (m_fileHandle != OS_INVALID_FILE) {
#if defined(_WIN32)
      ::CloseHandle(m_fileHandle);
#else //!_WIN32
      ::close(m_fileHandle);
#endif //!_WIN32
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

  [[nodiscard]] auto isValid() noexcept -> bool { return m_fileHandle != OS_INVALID_FILE; }

  auto readString(ExecutorHandle* execHandle, PlatformError* pErr, StringRef* str) noexcept
      -> bool {
    if (unlikely(str->getSize() == 0)) {
      return true;
    }

    execHandle->setState(ExecState::Paused);

#if defined(_WIN32)
    DWORD winBytesRead;
    const bool success =
        ::ReadFile(m_fileHandle, str->getCharDataPtr(), str->getSize(), &winBytesRead, nullptr);
    const int bytesRead = success ? static_cast<int>(winBytesRead) : -1;
#else //!_WIN32
    const int bytesRead = ::read(m_fileHandle, str->getCharDataPtr(), str->getSize());
#endif //!_WIN32

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

  auto readChar(ExecutorHandle* execHandle, PlatformError* pErr) noexcept -> char {
    execHandle->setState(ExecState::Paused);

    char res;
#if defined(_WIN32)
    DWORD winBytesRead;
    const bool success  = ::ReadFile(m_fileHandle, &res, 1, &winBytesRead, nullptr);
    const int bytesRead = success ? static_cast<int>(winBytesRead) : -1;
#else //!_WIN32
    const int bytesRead = ::read(m_fileHandle, &res, 1);
#endif //!_WIN32

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return '\0'; // Aborted.
    }

    if (bytesRead != 1) {
      *pErr = getFilePlatformError();
      return '\0';
    }
    if (bytesRead == 0) {
      *pErr = PlatformError::StreamNoDataAvailable;
      return '\0';
    }
    return res;
  }

  auto writeString(ExecutorHandle* execHandle, PlatformError* pErr, StringRef* str) noexcept
      -> bool {

    if (unlikely(str->getSize() == 0)) {
      return true;
    }

    execHandle->setState(ExecState::Paused);

#if defined(_WIN32)
    DWORD winBytesWritten;
    const bool success =
        ::WriteFile(m_fileHandle, str->getCharDataPtr(), str->getSize(), &winBytesWritten, nullptr);
    const int bytesWritten = success ? static_cast<int>(winBytesWritten) : -1;
#else //!_WIN32
    const int bytesWritten = ::write(m_fileHandle, str->getCharDataPtr(), str->getSize());
#endif //!_WIN32

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

  auto writeChar(ExecutorHandle* execHandle, PlatformError* pErr, uint8_t val) noexcept -> bool {

    execHandle->setState(ExecState::Paused);

    auto* valChar = static_cast<char*>(static_cast<void*>(&val));

#if defined(_WIN32)
    DWORD winBytesWritten;
    const bool success     = ::WriteFile(m_fileHandle, valChar, 1, &winBytesWritten, nullptr);
    const int bytesWritten = success ? static_cast<int>(winBytesWritten) : -1;
#else //!_WIN32
    const int bytesWritten = ::write(m_fileHandle, valChar, 1);
#endif //!_WIN32

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    if (bytesWritten != 1) {
      *pErr = getFilePlatformError();
      return false;
    }
    return true;
  }

  auto flush(PlatformError* /*unused*/) noexcept -> bool {
    // At the moment this is a no-op, in the future we can consider adding additional buffering to
    // file streams (so flush would write to the file handle). Or alternatively flush could ask the
    // os to actually write the data to disk, on linux with 'syncfs' or example.
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
  OS_FILE m_fileHandle;
  gsl::owner<char*> m_filePath;

  inline explicit FileStreamRef(
      OS_FILE fileHandle, gsl::owner<char*> filePath, FileStreamFlags flags) noexcept :
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
    creationDisposition |= OPEN_EXISTING;
    break;
  case FileStreamMode::Append:
    desiredAccess |= FILE_APPEND_DATA;
    creationDisposition |= OPEN_EXISTING;
    break;
  case FileStreamMode::Create:
    desiredAccess |= GENERIC_READ | GENERIC_WRITE;
    creationDisposition |= CREATE_ALWAYS;
    break;
  }

  const OS_FILE file = ::CreateFileA(
      filePath, desiredAccess, shareMode, nullptr, creationDisposition, flags, nullptr);
  if (file == OS_INVALID_FILE) {
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
  const OS_FILE file = ::open(filePath, flags, newFilePerms);
  if (file == OS_INVALID_FILE) {
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

inline auto removeFile(PlatformError* pErr, StringRef* path) -> bool {
#if defined(_WIN32)

  const bool success = ::DeleteFileA(path->getCharDataPtr());
  if (!success) {
    switch (::GetLastError()) {
    case ERROR_ACCESS_DENIED:
      *pErr = PlatformError::FileNoAccess;
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
    case ENOTDIR:
      *pErr = PlatformError::FileNotFound;
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
