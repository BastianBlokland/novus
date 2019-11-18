#include "prog/sym/type_id.hpp"

namespace prog::sym {

TypeId::TypeId(unsigned int id) : m_id{id} {}

auto TypeId::inferredType() -> TypeId { return TypeId{0}; };

auto TypeId::operator==(const TypeId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto TypeId::operator!=(const TypeId& rhs) const noexcept -> bool {
  return !TypeId::operator==(rhs);
}

auto TypeId::isInferred() const noexcept -> bool { return m_id == 0; };

auto operator<<(std::ostream& out, const TypeId& rhs) -> std::ostream& {
  return out << "t-" << rhs.m_id;
}

} // namespace prog::sym
