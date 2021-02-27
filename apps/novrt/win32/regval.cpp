#include "regval.hpp"
#if defined(_WIN32)

namespace novrt::win32 {

RegStrVal::RegStrVal(std::string val) : m_valid{true}, m_val{std::move(val)} {}

RegStrVal::RegStrVal(WinErr errCode) : m_valid{false}, m_errCode{errCode} {}

RegStrVal::~RegStrVal() noexcept {
  if (m_valid) {
    m_val.~basic_string();
  }
}

RegStrVal::RegStrVal(RegStrVal&& rhs) noexcept {
  if (rhs.m_valid) {
    std::swap(m_val, rhs.m_val);
    m_valid = true;
  } else {
    m_valid   = false;
    m_errCode = rhs.m_errCode;
  }
}

auto RegStrVal::operator=(RegStrVal&& rhs) noexcept -> RegStrVal& {
  if (this != &rhs) {
    if (rhs.m_valid) {
      m_valid = true;
      m_val   = std::move(rhs.m_val);
    } else {
      if (m_valid) {
        m_val.~basic_string();
      }
      m_valid   = false;
      m_errCode = rhs.m_errCode;
    }
  }
  return *this;
}

} // namespace novrt::win32

#endif // _WIN32
