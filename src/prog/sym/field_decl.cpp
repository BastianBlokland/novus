#include "prog/sym/field_decl.hpp"

namespace prog::sym {

FieldDecl::FieldDecl(FieldId id, std::string name, TypeId type) :
    m_id{id}, m_name{std::move(name)}, m_type{type} {}

auto FieldDecl::operator==(const FieldDecl& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto FieldDecl::operator!=(const FieldDecl& rhs) const noexcept -> bool {
  return !FieldDecl::operator==(rhs);
}

auto FieldDecl::getId() const -> const FieldId& { return m_id; }

auto FieldDecl::getName() const -> const std::string& { return m_name; }

auto FieldDecl::getType() const -> const TypeId& { return m_type; }

auto operator<<(std::ostream& out, const FieldDecl& rhs) -> std::ostream& {
  return out << '[' << rhs.m_id << "]-'" << rhs.m_name << "'-" << rhs.m_type;
}

} // namespace prog::sym
