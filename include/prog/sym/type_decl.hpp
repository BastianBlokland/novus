#pragma once
#include "prog/sym/type_id.hpp"
#include "prog/sym/type_kind.hpp"
#include <string>

namespace prog::sym {

// Type declaration. Combines the type-identifier, name and the type kind.
class TypeDecl final {
  friend class TypeDeclTable;
  friend auto operator<<(std::ostream& out, const TypeDecl& rhs) -> std::ostream&;

public:
  TypeDecl() = delete;

  auto operator==(const TypeDecl& rhs) const noexcept -> bool { return m_id == rhs.m_id; }
  auto operator!=(const TypeDecl& rhs) const noexcept -> bool { return !TypeDecl::operator==(rhs); }

  [[nodiscard]] auto getId() const -> const TypeId& { return m_id; }
  [[nodiscard]] auto getKind() const -> const TypeKind& { return m_kind; }
  [[nodiscard]] auto getName() const -> const std::string& { return m_name; }

private:
  TypeId m_id;
  TypeKind m_kind;
  std::string m_name;

  TypeDecl(TypeId id, TypeKind kind, std::string name) :
      m_id{id}, m_kind{kind}, m_name{std::move(name)} {}
};

auto operator<<(std::ostream& out, const TypeDecl& rhs) -> std::ostream&;

} // namespace prog::sym
