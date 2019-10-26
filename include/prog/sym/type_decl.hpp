#pragma once
#include "prog/sym/type_id.hpp"
#include "prog/sym/type_kind.hpp"
#include <string>

namespace prog::sym {

class TypeDecl final {
  friend class TypeDeclTable;
  friend auto operator<<(std::ostream& out, const TypeDecl& rhs) -> std::ostream&;

public:
  TypeDecl() = delete;

  auto operator==(const TypeDecl& rhs) const noexcept -> bool;
  auto operator!=(const TypeDecl& rhs) const noexcept -> bool;

  [[nodiscard]] auto getId() const -> const TypeId&;
  [[nodiscard]] auto getKind() const -> const TypeKind&;
  [[nodiscard]] auto getName() const -> const std::string&;

private:
  TypeId m_id;
  TypeKind m_kind;
  std::string m_name;

  TypeDecl(TypeId id, TypeKind kind, std::string name);
};

auto operator<<(std::ostream& out, const TypeDecl& rhs) -> std::ostream&;

} // namespace prog::sym
