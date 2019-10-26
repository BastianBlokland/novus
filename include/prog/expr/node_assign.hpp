#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/const_decl_table.hpp"
#include "prog/sym/const_id.hpp"

namespace prog::expr {

class AssignExprNode final : public Node {
  friend auto
  assignExprNode(const sym::ConstDeclTable& constTable, sym::ConstId constId, NodePtr expr)
      -> NodePtr;

public:
  AssignExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;

private:
  sym::ConstId m_constId;
  NodePtr m_expr;

  AssignExprNode(sym::ConstId constId, NodePtr expr);
};

// Factories.
auto assignExprNode(const sym::ConstDeclTable& constTable, sym::ConstId constId, NodePtr expr)
    -> NodePtr;

} // namespace prog::expr
