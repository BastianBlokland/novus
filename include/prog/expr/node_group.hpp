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

  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;

private:
  std::vector<NodePtr> m_exprs;

  explicit GroupExprNode(std::vector<NodePtr> exprs);
};

// Factories.
auto groupExprNode(std::vector<NodePtr> exprs) -> NodePtr;

} // namespace prog::expr
