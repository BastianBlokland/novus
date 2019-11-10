#pragma once
#include "prog/sym/action_decl.hpp"
#include "prog/sym/action_id.hpp"
#include "prog/sym/input.hpp"
#include <string>
#include <unordered_map>
#include <vector>

namespace prog::sym {

class ActionDeclTable final {
public:
  using iterator = typename std::vector<ActionDecl>::const_iterator;

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
  [[nodiscard]] auto lookup(const std::string& name, const Input& input) const
      -> std::optional<ActionId>;

  auto registerIntrinsic(std::string name, Input input) -> ActionId;

private:
  std::vector<ActionDecl> m_actions;
  std::unordered_map<std::string, std::vector<ActionId>> m_lookup;

  [[nodiscard]] auto findOverload(const std::vector<ActionId>& overloads, const Input& input) const
      -> std::optional<ActionId>;
};

} // namespace prog::sym
