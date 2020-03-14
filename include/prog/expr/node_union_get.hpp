#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

class UnionGetExprNode final : public Node {
  friend auto unionGetExprNode(
      const Program& prog, NodePtr lhs, const sym::ConstDeclTable& constTable, sym::ConstId constId)
      -> NodePtr;

public:
  UnionGetExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getConst() const noexcept -> sym::ConstId;
  [[nodiscard]] auto getTargetType() const noexcept -> sym::TypeId;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_boolType;
  NodePtr m_lhs;
  sym::TypeId m_targetType;
  sym::ConstId m_constId;

  UnionGetExprNode(sym::TypeId boolType, NodePtr lhs, sym::TypeId targetType, sym::ConstId constId);
};

// Factories.
auto unionGetExprNode(
    const Program& prog, NodePtr lhs, const sym::ConstDeclTable& constTable, sym::ConstId constId)
    -> NodePtr;

} // namespace prog::expr
