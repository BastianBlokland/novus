#if defined(_WIN32)
#include "utilities.hpp"
#include "Windows.h"

namespace {

[[noreturn]] auto errorExit(const char* msg) noexcept {
  std::fprintf(stderr, "%s\n", msg);
  std::fflush(stderr);
  std::exit(EXIT_FAILURE);
}

} // namespace

namespace novrt::win32 {

auto getExecutablePath() noexcept -> filesystem::path {
  char pathBuffer[MAX_PATH];
  auto size = GetModuleFileName(nullptr, pathBuffer, MAX_PATH);
  if (size == 0) {
    errorExit("Failed to retreive executable filename");
  }
  return filesystem::path{pathBuffer};
}

} // namespace novrt::win32

#endif // _Win32
