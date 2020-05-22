#pragma once
#include "internal/stream_opts.hpp"
#include "internal/os_include.hpp"
#include <cstdio>

namespace vm::internal {

// Unfortunately on windows file handles are not as generic as on unix so things
// like setting them to 'non-blocking' are not possible.
#if !defined(_WIN32)

// Set file-descriptor options.
inline auto setFileOpts(int fd, StreamOpts opts) noexcept -> bool {
  // Get the current options of the file descriptor.
  int fileOpts = fcntl(fd, F_GETFL);
  if (fileOpts < 0) {
    return false;
  }

  // Update the options.
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(StreamOpts::NoBlock)) {
    fileOpts |= O_NONBLOCK;
  }

  // Set the file options.
  if (fcntl(fd, F_SETFL, fileOpts) < 0) {
    return false;
  }
  return true;
}

// Set file-descriptor options.
inline auto setFileOpts(FILE* filePtr, StreamOpts opts) noexcept -> bool {
  return setFileOpts(fileno(filePtr), opts);
}

// Unset file-descriptor options.
inline auto unsetFileOpts(int fd, StreamOpts opts) noexcept -> bool {

  // Get the current options of the file descriptor.
  int fileOpts = fcntl(fd, F_GETFL);
  if (fileOpts < 0) {
    return false;
  }

  // Update the options.
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(StreamOpts::NoBlock)) {
    fileOpts &= ~O_NONBLOCK;
  }

  // Set the file options.
  if (fcntl(fd, F_SETFL, fileOpts) < 0) {
    return false;
  }
  return true;
}

// Unset file-descriptor options.
inline auto unsetFileOpts(FILE* filePtr, StreamOpts opts) noexcept -> bool {
  return unsetFileOpts(fileno(filePtr), opts);
}

#endif

} // namespace vm::internal
