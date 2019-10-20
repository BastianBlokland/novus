#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class BinaryExprNode final : public Node {
public:
  BinaryExprNode() = delete;
  BinaryExprNode(NodePtr lhs, lex::Token op, NodePtr rhs);

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> unsigned int override;
  [[nodiscard]] auto getOperator() const -> const lex::Token&;

private:
  const NodePtr m_lhs;
  const lex::Token m_op;
  const NodePtr m_rhs;

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto binaryExprNode(NodePtr lhs, lex::Token op, NodePtr rhs) -> NodePtr;

} // namespace parse
