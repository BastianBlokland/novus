#pragma once
#include "prog/sym/field_id.hpp"
#include "prog/sym/type_id.hpp"
#include <string>

namespace prog::sym {

// Field declaration. Combines a field-identifier, name and a type.
class FieldDecl final {
  friend class FieldDeclTable;
  friend auto operator<<(std::ostream& out, const FieldDecl& rhs) -> std::ostream&;

public:
  FieldDecl() = delete;

  auto operator==(const FieldDecl& rhs) const noexcept -> bool;
  auto operator!=(const FieldDecl& rhs) const noexcept -> bool;

  [[nodiscard]] auto getId() const -> const FieldId&;
  [[nodiscard]] auto getName() const -> const std::string&;
  [[nodiscard]] auto getType() const -> const sym::TypeId&;

private:
  FieldId m_id;
  std::string m_name;
  sym::TypeId m_type;

  FieldDecl(FieldId id, std::string name, sym::TypeId type);
};

auto operator<<(std::ostream& out, const FieldDecl& rhs) -> std::ostream&;

} // namespace prog::sym
