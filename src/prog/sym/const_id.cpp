#include "prog/sym/const_id.hpp"

namespace prog::sym {

ConstId::ConstId(unsigned int id) : m_id{id} {}

auto ConstId::operator==(const ConstId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto ConstId::operator!=(const ConstId& rhs) const noexcept -> bool {
  return !ConstId::operator==(rhs);
}

auto ConstId::getNum() const noexcept -> unsigned int { return m_id; }

auto operator<<(std::ostream& out, const ConstId& rhs) -> std::ostream& {
  return out << "c-" << rhs.m_id;
}

} // namespace prog::sym
