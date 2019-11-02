#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class UnaryExprNode final : public Node {
  friend auto unaryExprNode(lex::Token op, NodePtr rhs) -> NodePtr;

public:
  UnaryExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getOperator() const -> const lex::Token&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const lex::Token m_op;
  const NodePtr m_rhs;

  UnaryExprNode(lex::Token op, NodePtr rhs);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto unaryExprNode(lex::Token op, NodePtr rhs) -> NodePtr;

} // namespace parse
