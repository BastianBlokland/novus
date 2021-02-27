#pragma once
#if defined(_WIN32)
#include "filesystem.hpp"
#include <string>

namespace novrt::win32 {

[[nodiscard]] auto getExecutablePath() noexcept -> filesystem::path;

} // namespace novrt::win32

#endif // _Win32
