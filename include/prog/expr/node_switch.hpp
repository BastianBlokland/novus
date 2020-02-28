#pragma once
#include "prog/expr/node.hpp"
#include "prog/program.hpp"
#include <vector>

namespace prog::expr {

class SwitchExprNode final : public Node {
  friend auto switchExprNode(
      const Program& program, std::vector<NodePtr> conditions, std::vector<NodePtr> branches)
      -> NodePtr;

public:
  SwitchExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getType() const noexcept -> sym::TypeId override;
  [[nodiscard]] auto toString() const -> std::string override;

  [[nodiscard]] auto clone() const -> std::unique_ptr<Node> override;

  [[nodiscard]] auto getConditions() const noexcept -> const std::vector<NodePtr>&;
  [[nodiscard]] auto getBranches() const noexcept -> const std::vector<NodePtr>&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  std::vector<NodePtr> m_conditions;
  std::vector<NodePtr> m_branches;

  explicit SwitchExprNode(std::vector<NodePtr> conditions, std::vector<NodePtr> branches);
};

// Factories.
auto switchExprNode(
    const Program& program, std::vector<NodePtr> conditions, std::vector<NodePtr> branches)
    -> NodePtr;

} // namespace prog::expr
