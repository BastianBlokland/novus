#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class ConstDeclExprNode final : public Node {
public:
  ConstDeclExprNode() = delete;
  ConstDeclExprNode(lex::Token id, lex::Token eq, NodePtr rhs);

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;

private:
  const lex::Token m_id;
  const lex::Token m_eq;
  const NodePtr m_rhs;

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto constDeclExprNode(lex::Token id, lex::Token eq, NodePtr rhs) -> NodePtr;

} // namespace parse
