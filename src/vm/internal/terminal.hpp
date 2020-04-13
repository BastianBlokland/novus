#pragma once
#include <cstdint>
#include <termios.h>
#include <unistd.h>

namespace vm::internal {

enum class TermOpts : int32_t { Echo = 1 << 0, Buffering = 1 << 1 };

inline auto hasTerminal() -> bool {
#if defined(_WIN32)
  // TODO(bastian): Implement checking if terminal is active on windows.
  return false;
#else
  return isatty(0) == 1;
#endif
}

inline auto setTermOpts(TermOpts opts) -> bool {
  if (!hasTerminal()) {
    return false;
  }

#if defined(_WIN32)
  // TODO(bastian): Implement terminal control options for Windows.
#else
  /* Update the termios structure for file descriptor 0 (main terminal). */

  termios t;
  if (tcgetattr(0, &t) != 0) {
    return false;
  }

  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::Echo)) {
    t.c_lflag |= ECHO;
  }
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::Buffering)) {
    t.c_lflag |= ICANON;
  }

  if (tcsetattr(0, TCSANOW, &t) != 0) {
    return false;
  }
  return true;
#endif
}

inline auto unsetTermOpts(TermOpts opts) -> bool {
  if (!hasTerminal()) {
    return false;
  }

#if defined(_WIN32)
  // TODO(bastian): Implement terminal control options for Windows.
#else
  /* Update the termios structure for file descriptor 0 (main terminal). */

  termios t;
  if (tcgetattr(0, &t)) {
    return false;
  }

  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::Echo)) {
    t.c_lflag &= ~ECHO;
  }
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::Buffering)) {
    t.c_lflag &= ~ICANON;
  }

  if (tcsetattr(0, TCSANOW, &t)) {
    return false;
  }
  return true;
#endif
}

} // namespace vm::internal
