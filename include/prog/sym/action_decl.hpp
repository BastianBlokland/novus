#pragma once
#include "prog/sym/action_id.hpp"
#include "prog/sym/action_kind.hpp"
#include "prog/sym/input.hpp"
#include <string>

namespace prog::sym {

class ActionDecl final {
  friend class ActionDeclTable;
  friend auto operator<<(std::ostream& out, const ActionDecl& rhs) -> std::ostream&;

public:
  ActionDecl() = delete;

  auto operator==(const ActionDecl& rhs) const noexcept -> bool;
  auto operator!=(const ActionDecl& rhs) const noexcept -> bool;

  [[nodiscard]] auto getId() const -> const ActionId&;
  [[nodiscard]] auto getKind() const -> const ActionKind&;
  [[nodiscard]] auto getName() const -> const std::string&;
  [[nodiscard]] auto getInput() const -> const Input&;

private:
  ActionId m_id;
  ActionKind m_kind;
  std::string m_name;
  Input m_input;

  ActionDecl(ActionId id, ActionKind kind, std::string name, Input input);
};

auto operator<<(std::ostream& out, const ActionDecl& rhs) -> std::ostream&;

} // namespace prog::sym