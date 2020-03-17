#pragma once
#include "prog/expr/node.hpp"
#include <vector>

namespace prog::expr {

class GroupExprNode final : public Node {
  friend auto groupExprNode(std::vector<NodePtr> exprs) -> NodePtr;

public:
  GroupExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] constexpr static auto getKind() { return NodeKind::Group; }

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone(Rewriter* rewriter) const -> std::unique_ptr<Node> override;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  std::vector<NodePtr> m_exprs;

  explicit GroupExprNode(std::vector<NodePtr> exprs);
};

// Factories.
auto groupExprNode(std::vector<NodePtr> exprs) -> NodePtr;

} // namespace prog::expr
