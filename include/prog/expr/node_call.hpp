#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/func_decl_table.hpp"

namespace prog::expr {

class CallExprNode final : public Node {
  friend auto
  callExprNode(const sym::FuncDeclTable& funcTable, sym::FuncId func, std::vector<NodePtr> args)
      -> NodePtr;

public:
  CallExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;

private:
  sym::FuncId m_func;
  sym::TypeId m_resultType;
  std::vector<NodePtr> m_args;

  CallExprNode(sym::FuncId func, sym::TypeId resultType, std::vector<NodePtr> args);
};

// Factories.
auto callExprNode(const sym::FuncDeclTable& funcTable, sym::FuncId func, std::vector<NodePtr> args)
    -> NodePtr;

} // namespace prog::expr
