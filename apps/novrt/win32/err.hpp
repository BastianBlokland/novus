#pragma once
#if defined(_WIN32)
#include <optional>
#include <string>

namespace novrt::win32 {

using WinErr    = long;
using OptWinErr = std::optional<WinErr>;

[[nodiscard]] auto inline winSuccess() noexcept -> OptWinErr { return std::nullopt; }

[[nodiscard]] auto winErrToString(WinErr errCode) noexcept -> std::string;

} // namespace novrt::win32

#endif // _Win32
