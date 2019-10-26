#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/type_decl_table.hpp"

namespace prog::expr {

class LitBoolNode final : public Node {
  friend auto litBoolNode(const sym::TypeDeclTable& typeTable, bool val) -> NodePtr;

public:
  LitBoolNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;

private:
  sym::TypeId m_type;
  bool m_val;

  LitBoolNode(sym::TypeId type, bool val);
};

// Factories.
auto litBoolNode(const sym::TypeDeclTable& typeTable, bool val) -> NodePtr;

} // namespace prog::expr
