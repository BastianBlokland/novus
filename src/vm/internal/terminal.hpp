#pragma once
#include <cstdint>

#if !defined(_WIN32)

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#endif

namespace vm::internal {

enum class TermOpts : int32_t { NoEcho = 1 << 0, NoBuffer = 1 << 1 };

inline auto hasTerminal() -> bool {
#if defined(_WIN32)
  // TODO(bastian): Implement checking if terminal is active on windows.
  return false;
#else
  return isatty(0) == 1;
#endif
}

inline auto termGetWidth() -> int32_t {
  if (!hasTerminal()) {
    return -1;
  }

#if defined(_WIN32)
  // TODO(bastian): Implement terminal get width on windows.
  return -1;
#else
  winsize ws;
  if (ioctl(0, TIOCGWINSZ, &ws)) {
    return -1;
  }
  return static_cast<int32_t>(ws.ws_col);
#endif
}

inline auto termGetHeight() -> int32_t {
  if (!hasTerminal()) {
    return -1;
  }

#if defined(_WIN32)
  // TODO(bastian): Implement terminal get height on windows.
  return -1;
#else
  winsize ws;
  if (ioctl(0, TIOCGWINSZ, &ws)) {
    return -1;
  }
  return static_cast<int32_t>(ws.ws_row);
#endif
}

inline auto termSetOpts(TermOpts opts) -> bool {
  if (!hasTerminal()) {
    return false;
  }

#if defined(_WIN32)
  // TODO(bastian): Implement terminal control options for Windows.
#else
  /* Update the termios structure for file descriptor 0 (std-in). */

  termios t;
  if (tcgetattr(0, &t) != 0) {
    return false;
  }

  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoEcho)) {
    t.c_lflag &= ~ECHO;
  }
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoBuffer)) {
    t.c_lflag &= ~ICANON;
  }

  if (tcsetattr(0, TCSANOW, &t) != 0) {
    return false;
  }
  return true;
#endif
}

inline auto termUnsetOpts(TermOpts opts) -> bool {
  if (!hasTerminal()) {
    return false;
  }

#if defined(_WIN32)
  // TODO(bastian): Implement terminal control options for Windows.
#else
  /* Update the termios structure for file descriptor 0 (std-in). */

  termios t;
  if (tcgetattr(0, &t)) {
    return false;
  }

  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoEcho)) {
    t.c_lflag |= ECHO;
  }
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoBuffer)) {
    t.c_lflag |= ICANON;
  }

  if (tcsetattr(0, TCSANOW, &t)) {
    return false;
  }
  return true;
#endif
}

} // namespace vm::internal
