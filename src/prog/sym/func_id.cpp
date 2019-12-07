#include "prog/sym/func_id.hpp"

namespace prog::sym {

FuncId::FuncId(unsigned int id) : m_id{id} {}

auto FuncId::operator==(const FuncId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto FuncId::operator!=(const FuncId& rhs) const noexcept -> bool {
  return !FuncId::operator==(rhs);
}

auto FuncId::getNum() const noexcept -> unsigned int { return m_id; }

auto operator<<(std::ostream& out, const FuncId& rhs) -> std::ostream& {
  return out << "f-" << rhs.m_id;
}

} // namespace prog::sym
