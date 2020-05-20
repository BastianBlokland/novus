#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

// Parenthesized expression, used to delclare precedence over other operators.
// Example in source: '(a + b)'.
class ParenExprNode final : public Node {
  friend auto parenExprNode(lex::Token open, NodePtr expr, lex::Token close) -> NodePtr;

public:
  ParenExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_open;
  const NodePtr m_expr;
  const lex::Token m_close;

  ParenExprNode(lex::Token open, NodePtr expr, lex::Token close);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto parenExprNode(lex::Token open, NodePtr expr, lex::Token close) -> NodePtr;

} // namespace parse
