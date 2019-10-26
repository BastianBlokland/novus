#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/type_decl_table.hpp"
#include <vector>

namespace prog::expr {

class BranchExprNode final : public Node {
  friend auto branchExprNode(
      const sym::TypeDeclTable& typeTable,
      std::vector<NodePtr> conditions,
      std::vector<NodePtr> branches) -> NodePtr;

public:
  BranchExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;

private:
  std::vector<NodePtr> m_conditions;
  std::vector<NodePtr> m_branches;

  explicit BranchExprNode(std::vector<NodePtr> conditions, std::vector<NodePtr> branches);
};

// Factories.
auto branchExprNode(
    const sym::TypeDeclTable& typeTable,
    std::vector<NodePtr> conditions,
    std::vector<NodePtr> branches) -> NodePtr;

} // namespace prog::expr
