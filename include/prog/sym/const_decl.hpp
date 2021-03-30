#pragma once
#include "prog/sym/const_id.hpp"
#include "prog/sym/const_kind.hpp"
#include "prog/sym/type_id.hpp"
#include <string>

namespace prog::sym {

// Declaration of a constant, combines a constant-id, name and a type.
class ConstDecl final {
  friend class ConstDeclTable;
  friend auto operator<<(std::ostream& out, const ConstDecl& rhs) -> std::ostream&;

public:
  ConstDecl() = delete;

  auto operator==(const ConstDecl& rhs) const noexcept -> bool { return m_id == rhs.m_id; }
  auto operator!=(const ConstDecl& rhs) const noexcept -> bool {
    return !ConstDecl::operator==(rhs);
  }

  [[nodiscard]] auto getId() const -> const ConstId& { return m_id; }
  [[nodiscard]] auto getKind() const -> const ConstKind& { return m_kind; }
  [[nodiscard]] auto getName() const -> const std::string& { return m_name; }
  [[nodiscard]] auto getType() const -> const TypeId& { return m_type; }

private:
  ConstId m_id;
  ConstKind m_kind;
  std::string m_name;
  TypeId m_type;

  ConstDecl(ConstId id, ConstKind kind, std::string name, TypeId type) :
      m_id{id}, m_kind{kind}, m_name{std::move(name)}, m_type{type} {}
};

auto operator<<(std::ostream& out, const ConstDecl& rhs) -> std::ostream&;

} // namespace prog::sym
