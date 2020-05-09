#pragma once
#include "internal/ref_stream.hpp"

#if !defined(_WIN32)

#include <cstdio>
#include <fcntl.h>

#endif

namespace vm::internal {

#if defined(_WIN32)

inline auto setFileOpts(FILE* /*unused*/, StreamOpts /*unused*/) noexcept -> bool {
  // TODO(bastian): Implement stream options for windows.
  return false;
}

inline auto unsetFileOpts(FILE* /*unused*/, StreamOpts /*unused*/) noexcept -> bool {
  // TODO(bastian): Implement stream options for windows.
  return false;
}

#else

inline auto setFileOpts(FILE* filePtr, StreamOpts opts) noexcept -> bool {
  // Get the current options of the file descriptor.
  auto fd      = fileno(filePtr);
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

inline auto unsetFileOpts(FILE* filePtr, StreamOpts opts) noexcept -> bool {

  // Get the current options of the file descriptor.
  auto fd      = fileno(filePtr);
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

#endif

} // namespace vm::internal
