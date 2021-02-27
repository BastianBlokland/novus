#pragma once
#if defined(_WIN32)
#include "err.hpp"
#include <string>

namespace novrt::win32 {

class RegStrVal {
public:
  explicit RegStrVal(std::string val);
  explicit RegStrVal(WinErr errCode);

  RegStrVal(const RegStrVal& rhs) = delete;

  RegStrVal(RegStrVal&& rhs) noexcept;

  ~RegStrVal() noexcept;

  auto operator=(const RegStrVal& rhs) -> RegStrVal& = delete;

  auto operator=(RegStrVal&& rhs) noexcept -> RegStrVal&;

  operator bool() const noexcept { return m_valid; }
  auto operator*() -> std::string_view { return m_val; }

  [[nodiscard]] auto isValid() const noexcept { return m_valid; }

  [[nodiscard]] auto getVal() const noexcept -> std::string_view { return m_val; }

  [[nodiscard]] auto getErrorCode() const noexcept -> WinErr { return m_errCode; }
  [[nodiscard]] auto getErrorMsg() const noexcept -> std::string {
    return winErrToString(m_errCode);
  }

private:
  bool m_valid;
  union {
    std::string m_val;
    WinErr m_errCode;
  };
};

} // namespace novrt::win32

#endif // _WIN32
