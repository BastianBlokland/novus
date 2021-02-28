#pragma once
#include "err.hpp"
#if defined(_WIN32)

#include "Windows.h"

namespace novrt::win32 {

class RegKey {
public:
  enum class Disposition {
    New,
    Existing,
  };

  explicit RegKey(HKEY key, Disposition disposition);
  explicit RegKey(WinErr errCode);

  RegKey(const RegKey& rhs) = delete;

  RegKey(RegKey&& rhs) noexcept;

  ~RegKey() noexcept;

  auto operator=(const RegKey& rhs) -> RegKey& = delete;

  auto operator=(RegKey&& rhs) noexcept -> RegKey&;

  operator bool() const noexcept { return m_valid; }
  auto operator*() const -> HKEY { return m_key; }

  [[nodiscard]] auto isValid() const noexcept { return m_valid; }

  [[nodiscard]] auto getKey() const noexcept { return m_key; }
  [[nodiscard]] auto isNew() const noexcept { return m_disposition == Disposition::New; }
  [[nodiscard]] auto getDisposition() const noexcept { return m_disposition; }

  [[nodiscard]] auto getErrorCode() const noexcept -> WinErr { return m_errCode; }
  [[nodiscard]] auto getErrorMsg() const noexcept -> std::string {
    return winErrToString(m_errCode);
  }

private:
  bool m_valid;
  union {
    struct {
      HKEY m_key;
      Disposition m_disposition;
    };
    WinErr m_errCode;
  };
};

} // namespace novrt::win32

#endif // _WIN32
