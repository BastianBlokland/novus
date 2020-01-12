#pragma once
#include "prog/sym/action_decl.hpp"
#include "prog/sym/action_id.hpp"
#include "prog/sym/type_set.hpp"
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace prog {

class Program;

namespace sym {

class ActionDeclTable final {
public:
  using iterator = typename std::vector<ActionDecl>::const_iterator;
  using id       = typename prog::sym::ActionId;

  ActionDeclTable()                               = default;
  ActionDeclTable(const ActionDeclTable& rhs)     = delete;
  ActionDeclTable(ActionDeclTable&& rhs) noexcept = default;
  ~ActionDeclTable()                              = default;

  auto operator=(const ActionDeclTable& rhs) -> ActionDeclTable& = delete;
  auto operator=(ActionDeclTable&& rhs) noexcept -> ActionDeclTable& = delete;

  [[nodiscard]] auto operator[](ActionId id) const -> const ActionDecl&;

  [[nodiscard]] auto begin() const -> iterator;
  [[nodiscard]] auto end() const -> iterator;

  [[nodiscard]] auto lookup(const std::string& name) const -> std::vector<ActionId>;
  [[nodiscard]] auto lookup(
      const Program& prog, const std::string& name, const TypeSet& input, int maxConversions) const
      -> std::optional<ActionId>;

  auto registerAction(const Program& prog, ActionKind kind, std::string name, TypeSet input)
      -> ActionId;

private:
  std::vector<ActionDecl> m_actions;
  std::unordered_map<std::string, std::vector<ActionId>> m_lookup;
};

} // namespace sym

} // namespace prog
