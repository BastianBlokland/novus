#pragma once
#include <cstdint>

#if defined(_WIN32)

#include <windows.h>

#else // !_WIN32

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#endif

namespace vm::internal {

enum class TermOpts : int32_t { NoEcho = 1 << 0, NoBuffer = 1 << 1 };

inline auto hasTerminal() -> bool {
#if defined(_WIN32)
  return GetFileType(GetStdHandle(STD_OUTPUT_HANDLE)) == FILE_TYPE_CHAR;
#else // !_WIN32
  return isatty(0) == 1;
#endif
}

inline auto termGetWidth() -> int32_t {
  if (!hasTerminal()) {
    return -1;
  }

#if defined(_WIN32)

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    return -1;
  }
  return 1 + csbi.srWindow.Right - csbi.srWindow.Left;

#else // !_WIN32

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

  CONSOLE_SCREEN_BUFFER_INFO csbi;
  if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
    return -1;
  }
  return 1 + csbi.srWindow.Bottom - csbi.srWindow.Top;

#else // !_WIN32

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

    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    if (!GetConsoleMode(hStdin, &mode)) {
      return false;
    }

    if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoEcho)) {
      mode &= ~ENABLE_ECHO_INPUT;
    }
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoBuffer)) {
      mode &= ~ENABLE_LINE_INPUT;
    }

    return SetConsoleMode(hStdin, mode);

#else // !_WIN32

  /* Update the termios structure for file descriptor 0 (std-in). */
  termios t;
  if (tcgetattr(0, &t)) {
    return false;
  }

  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoEcho)) {
    t.c_lflag &= ~ECHO;
  }
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoBuffer)) {
    t.c_lflag &= ~ICANON;
  }

  return !tcsetattr(0, TCSANOW, &t);

#endif
}

inline auto termUnsetOpts(TermOpts opts) -> bool {
  if (!hasTerminal()) {
    return false;
  }

#if defined(_WIN32)

  HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
  DWORD mode = 0;
  if (!GetConsoleMode(hStdin, &mode)) {
    return false;
  }

  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoEcho)) {
    mode |= ENABLE_ECHO_INPUT;
  }
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoBuffer)) {
    mode |= ENABLE_LINE_INPUT;
  }

  return SetConsoleMode(hStdin, mode);

#else // !_WIN32

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

  return !tcsetattr(0, TCSANOW, &t);

#endif
}


} // namespace vm::internal
