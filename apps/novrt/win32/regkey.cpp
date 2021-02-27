#if defined(_WIN32)
#include "regkey.hpp"

namespace novrt::win32 {

RegKey::RegKey(HKEY key, Disposition disposition) :
    m_valid{true}, m_key{key}, m_disposition{disposition} {}

RegKey::RegKey(WinErr errCode) : m_valid{false}, m_errCode{errCode} {}

RegKey::RegKey(RegKey&& rhs) noexcept {
  if (rhs.m_valid) {
    m_key         = rhs.m_key;
    m_disposition = rhs.m_disposition;
    m_valid       = true;
    rhs.m_valid   = false;
    rhs.m_errCode = WinErr{};
  } else {
    m_valid   = false;
    m_errCode = rhs.m_errCode;
  }
}

RegKey::~RegKey() noexcept {
  if (m_valid) {
    ::RegCloseKey(m_key);
  }
}

auto RegKey::operator=(RegKey&& rhs) noexcept -> RegKey& {
  if (this != &rhs) {
    if (rhs.m_valid) {
      m_key         = rhs.m_key;
      m_disposition = rhs.m_disposition;
      m_valid       = true;
      rhs.m_valid   = false;
      rhs.m_errCode = WinErr{};
    } else {
      m_valid   = false;
      m_errCode = rhs.m_errCode;
    }
  }
  return *this;
}

} // namespace novrt::win32

#endif // _WIN32
