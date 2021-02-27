#if defined(_WIN32)
#include "err.hpp"
#include "Windows.h"

namespace novrt::win32 {

auto winErrToString(WinErr errCode) noexcept -> std::string {
  // Translate the errCode into a string message.
  LPSTR msgBuffer = nullptr;
  const auto size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
      nullptr,
      errCode,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      reinterpret_cast<LPSTR>(&msgBuffer),
      0,
      nullptr);

  // Copy the buffer to a string object.
  auto result = std::string{msgBuffer, size};

  // Free the win32 buffer.
  LocalFree(msgBuffer);
  return result;
}

} // namespace novrt::win32

#endif // _Win32
