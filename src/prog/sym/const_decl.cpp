#include "prog/sym/const_decl.hpp"

namespace prog::sym {

ConstDecl::ConstDecl(ConstId id, ConstKind kind, std::string name, TypeId type) :
    m_id{id}, m_kind{kind}, m_name{std::move(name)}, m_type{type} {}

auto ConstDecl::operator==(const ConstDecl& rhs) const noexcept -> bool { return m_id == rhs.m_id; }

auto ConstDecl::operator!=(const ConstDecl& rhs) const noexcept -> bool {
  return !ConstDecl::operator==(rhs);
}

auto ConstDecl::getId() const -> const ConstId& { return m_id; }

auto ConstDecl::getKind() const -> const ConstKind& { return m_kind; }

auto ConstDecl::getName() const -> const std::string& { return m_name; }

auto ConstDecl::getType() const -> const TypeId& { return m_type; }

auto operator<<(std::ostream& out, const ConstDecl& rhs) -> std::ostream& {
  return out << '[' << rhs.m_id << "]-'" << rhs.m_name << "'-" << rhs.m_type;
}

} // namespace prog::sym
