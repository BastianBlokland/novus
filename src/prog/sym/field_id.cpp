#include "prog/sym/field_id.hpp"

namespace prog::sym {

FieldId::FieldId(unsigned int id) : m_id{id} {}

auto FieldId::operator==(const FieldId& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto FieldId::operator!=(const FieldId& rhs) const noexcept -> bool {
  return !FieldId::operator==(rhs);
}

auto FieldId::getNum() const noexcept -> unsigned int { return m_id; }

auto operator<<(std::ostream& out, const FieldId& rhs) -> std::ostream& {
  return out << "field-" << rhs.m_id;
}

} // namespace prog::sym
