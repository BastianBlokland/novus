#pragma once

namespace vm::internal {

struct Settings {
  bool socketsEnabled;
  bool interceptInterupt;

#if defined(_WIN32)
  unsigned long win32OriginalInputConsoleMode;
  unsigned long win32OriginalOutputConsoleMode;
#endif // _WIN32
};

} // namespace vm::internal
