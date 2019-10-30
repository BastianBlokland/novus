#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class ConstDeclExprNode final : public Node {
  friend auto constDeclExprNode(lex::Token id, lex::Token eq, NodePtr rhs) -> NodePtr;

public:
  ConstDeclExprNode() = delete;

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getSpan() const -> input::SourceSpan override;

  [[nodiscard]] auto getId() const -> const lex::Token&;

private:
  const lex::Token m_id;
  const lex::Token m_eq;
  const NodePtr m_rhs;

  ConstDeclExprNode(lex::Token id, lex::Token eq, NodePtr rhs);

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto constDeclExprNode(lex::Token id, lex::Token eq, NodePtr rhs) -> NodePtr;

} // namespace parse
