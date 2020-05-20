#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

// Conditional expression, also known as the 'ternary operator'.
// Example in source: 'a ? 42 : 43'.
class ConditionalExprNode final : public Node {
  friend auto conditionalExprNode(
      NodePtr cond, lex::Token qmark, NodePtr ifBranch, lex::Token colon, NodePtr elseBranch)
      -> NodePtr;

public:
  ConditionalExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const NodePtr m_cond;
  const lex::Token m_qmark;
  const NodePtr m_ifBranch;
  const lex::Token m_colon;
  const NodePtr m_elseBranch;

  ConditionalExprNode(
      NodePtr cond, lex::Token qmark, NodePtr ifBranch, lex::Token colon, NodePtr elseBranch);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto conditionalExprNode(
    NodePtr cond, lex::Token qmark, NodePtr ifBranch, lex::Token colon, NodePtr elseBranch)
    -> NodePtr;

} // namespace parse
