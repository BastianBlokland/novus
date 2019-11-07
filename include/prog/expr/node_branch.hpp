#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"
#include <vector>

namespace prog::expr {

class BranchExprNode final : public Node {
  friend auto branchExprNode(
      const Program& program, std::vector<NodePtr> conditions, std::vector<NodePtr> branches)
      -> NodePtr;

public:
  BranchExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  std::vector<NodePtr> m_conditions;
  std::vector<NodePtr> m_branches;

  explicit BranchExprNode(std::vector<NodePtr> conditions, std::vector<NodePtr> branches);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto branchExprNode(
    const Program& program, std::vector<NodePtr> conditions, std::vector<NodePtr> branches)
    -> NodePtr;

} // namespace prog::expr