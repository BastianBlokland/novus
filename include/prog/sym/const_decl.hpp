#pragma once
#include "prog/sym/const_id.hpp"
#include "prog/sym/const_kind.hpp"
#include "prog/sym/type_id.hpp"
#include <string>

namespace prog::sym {

class ConstDecl final {
  friend class ConstDeclTable;
  friend auto operator<<(std::ostream& out, const ConstDecl& rhs) -> std::ostream&;

public:
  ConstDecl() = delete;

  auto operator==(const ConstDecl& rhs) const noexcept -> bool;
  auto operator!=(const ConstDecl& rhs) const noexcept -> bool;

  [[nodiscard]] auto getId() const -> const ConstId&;
  [[nodiscard]] auto getKind() const -> const ConstKind&;
  [[nodiscard]] auto getName() const -> const std::string&;
  [[nodiscard]] auto getType() const -> const TypeId&;

private:
  ConstId m_id;
  ConstKind m_kind;
  std::string m_name;
  TypeId m_type;

  ConstDecl(ConstId id, ConstKind kind, std::string name, TypeId type);
};

auto operator<<(std::ostream& out, const ConstDecl& rhs) -> std::ostream&;

} // namespace prog::sym
