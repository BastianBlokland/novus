#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class FieldExprNode final : public Node {
  friend auto fieldExprNode(NodePtr lhs, lex::Token dot, lex::Token id) -> NodePtr;

public:
  FieldExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](unsigned int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::Span override;

  [[nodiscard]] auto getId() const -> const lex::Token&;

  auto accept(NodeVisitor* visitor) const -> void override;

private:
  const NodePtr m_lhs;
  const lex::Token m_dot;
  const lex::Token m_id;

  FieldExprNode(NodePtr lhs, lex::Token dot, lex::Token id);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto fieldExprNode(NodePtr lhs, lex::Token dot, lex::Token id) -> NodePtr;

} // namespace parse
