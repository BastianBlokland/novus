#include "prog/sym/type_decl.hpp"

namespace prog::sym {

TypeDecl::TypeDecl(TypeId id, TypeKind kind, std::string name) :
    m_id{id}, m_kind{kind}, m_name{std::move(name)} {}

auto TypeDecl::operator==(const TypeDecl& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto TypeDecl::operator!=(const TypeDecl& rhs) const noexcept -> bool {
  return !TypeDecl::operator==(rhs);
}

auto TypeDecl::getId() const -> const TypeId& { return m_id; }

auto TypeDecl::getKind() const -> const TypeKind& { return m_kind; }

auto TypeDecl::getName() const -> const std::string& { return m_name; }

auto operator<<(std::ostream& out, const TypeDecl& rhs) -> std::ostream& {
  return out << '[' << rhs.m_id << "]-'" << rhs.m_name << '\'';
}

} // namespace prog::sym
