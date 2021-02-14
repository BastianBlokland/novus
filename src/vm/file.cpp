#include "vm/file.hpp"
#include "internal/os_include.hpp"
#include "internal/thread.hpp"
#include <array>
#include <cstring>

namespace vm {

#if defined(_WIN32)

auto fileInvalid() noexcept -> FileHandle { return INVALID_HANDLE_VALUE; }
auto fileIsValid(FileHandle handle) noexcept -> bool { return handle != INVALID_HANDLE_VALUE; }

auto fileStdIn() noexcept -> FileHandle { return ::GetStdHandle(STD_INPUT_HANDLE); }
auto fileStdOut() noexcept -> FileHandle { return ::GetStdHandle(STD_OUTPUT_HANDLE); }
auto fileStdErr() noexcept -> FileHandle { return ::GetStdHandle(STD_ERROR_HANDLE); }

auto fileTemp() noexcept -> FileHandle {
  std::array<char, MAX_PATH> tempDirName;
  std::array<char, MAX_PATH> tempFileName;

  const auto tempDirLen = GetTempPathA(MAX_PATH, tempDirName.data());
  if (tempDirLen == 0 || tempDirLen > MAX_PATH) {
    return nullptr;
  }

  if (GetTempFileName(tempDirName.data(), TEXT("novtmp"), 0, tempFileName.data()) == 0) {
    return nullptr;
  }

  return CreateFile(
      tempFileName.data(),
      GENERIC_READ | GENERIC_WRITE,
      0,
      nullptr,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_DELETE_ON_CLOSE,
      nullptr);
}

auto fileRead(FileHandle file, char* buffer, size_t bufferSize) noexcept -> int {
  DWORD bytesRead;
  if (::ReadFile(file, buffer, bufferSize, &bytesRead, nullptr)) {
    return static_cast<int>(bytesRead);
  }
  return -1;
}

auto fileWrite(FileHandle file, const char* buffer, size_t bufferSize) noexcept -> int {
  DWORD bytesWritten;
  if (::WriteFile(file, buffer, bufferSize, &bytesWritten, nullptr)) {
    return static_cast<int>(bytesWritten);
  }
  return -1;
}

auto fileSeek(FileHandle file, size_t position) noexcept -> bool {
  LARGE_INTEGER li;
  li.QuadPart = position;
  li.LowPart  = SetFilePointer(file, li.LowPart, &li.HighPart, FILE_BEGIN);
  if (li.LowPart == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
    li.QuadPart = -1;
  }
  return li.QuadPart == static_cast<long long>(position);
}

auto fileClose(FileHandle file) noexcept -> void { ::CloseHandle(file); }

#else // !_WIN32

auto fileInvalid() noexcept -> FileHandle { return -1; }
auto fileIsValid(FileHandle handle) noexcept -> bool { return handle >= 0; }

auto fileStdIn() noexcept -> FileHandle { return 0; }
auto fileStdOut() noexcept -> FileHandle { return 1; }
auto fileStdErr() noexcept -> FileHandle { return 2; }

auto fileTemp() noexcept -> FileHandle {
  const char* nameTemplate = "novtmp_XXXXXX";
  char* name               = static_cast<char*>(::malloc(::strlen(nameTemplate) + 1));
  ::strcpy(name, nameTemplate);
  FileHandle result = ::mkstemp(name);
  if (fileIsValid(result)) {
    ::unlink(name);
  }
  ::free(name);
  return result;
}

auto fileRead(FileHandle file, char* buffer, size_t bufferSize) noexcept -> int {
  int bytesRead;
  while (true) {
    bytesRead = ::read(file, buffer, bufferSize);
    if (bytesRead >= 0) {
      break; // Success.
    }
    bool shouldRetry = false;
    switch (errno) {
    case EINTR:
      shouldRetry = true;
      break;
    }
    if (!shouldRetry) {
      break; // Not an error we should retry.
    }
    internal::threadYield(); // Yield between retries.
  }
  return bytesRead;
}

auto fileWrite(FileHandle file, const char* buffer, size_t bufferSize) noexcept -> int {
  int bytesWritten;
  while (true) {
    bytesWritten = ::write(file, buffer, bufferSize);
    if (bytesWritten == static_cast<int>(bufferSize)) {
      break; // Success.
    }
    bool shouldRetry = false;
    switch (errno) {
    case EAGAIN:
    case EINTR:
      shouldRetry = true;
      break;
    }
    if (!shouldRetry) {
      break; // Not an error we should retry.
    }
    internal::threadYield(); // Yield between retries.
  }
  return bytesWritten;
}

auto fileSeek(FileHandle file, size_t position) noexcept -> bool {
  return static_cast<size_t>(::lseek(file, position, SEEK_SET)) == position;
}

auto fileClose(FileHandle file) noexcept -> void { ::close(file); }

#endif // !_WIN32

} // namespace vm
