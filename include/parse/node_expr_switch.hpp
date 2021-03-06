#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"
#include <vector>

namespace parse {

// Switch expression.
// Example in source:
// '
// if a > 42  -> 1
// if b < 1   -> 2
// else       -> 3
// '
class SwitchExprNode final : public Node {
  friend auto switchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause) -> NodePtr;

public:
  SwitchExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto hasElse() const -> bool;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const std::vector<NodePtr> m_ifClauses;
  const NodePtr m_elseClause;

  SwitchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto switchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause) -> NodePtr;

} // namespace parse
