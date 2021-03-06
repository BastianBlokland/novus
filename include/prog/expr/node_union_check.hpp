#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"

namespace prog::expr {

// Check if the union holds a value of type 'targetType'.
class UnionCheckExprNode final : public Node {
  friend auto unionCheckExprNode(const Program& prog, NodePtr lhs, sym::TypeId targetType)
      -> NodePtr;

public:
  UnionCheckExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::UnionCheck; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getTargetType() const noexcept -> sym::TypeId;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  sym::TypeId m_boolType;
  NodePtr m_lhs;
  sym::TypeId m_targetType;

  UnionCheckExprNode(sym::TypeId boolType, NodePtr lhs, sym::TypeId targetType);
};

// Factories.
auto unionCheckExprNode(const Program& prog, NodePtr lhs, sym::TypeId targetType) -> NodePtr;

} // namespace prog::expr
