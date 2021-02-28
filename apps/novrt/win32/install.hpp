#pragma once
#if defined(_WIN32)

namespace novrt::win32 {

auto registerWin32FileAssoc() noexcept -> bool;

auto unregisterWin32FileAssoc() noexcept -> bool;

} // namespace novrt::win32

#endif // _Win32
