#include "prog/sym/action_id.hpp"

namespace prog::sym {

ActionId::ActionId(unsigned int id) : m_id{id} {}

auto ActionId::operator==(const ActionId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto ActionId::operator!=(const ActionId& rhs) const noexcept -> bool {
  return !ActionId::operator==(rhs);
}

auto operator<<(std::ostream& out, const ActionId& rhs) -> std::ostream& {
  return out << "a-" << rhs.m_id;
}

} // namespace prog::sym
