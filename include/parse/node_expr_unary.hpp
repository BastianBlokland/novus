#pragma once
#include "lex/token.hpp"
#include "parse/node.hpp"

namespace parse {

class UnaryExprNode final : public Node {
public:
  UnaryExprNode() = delete;
  UnaryExprNode(lex::Token op, NodePtr rhs);

  auto operator==(const Node& rhs) const noexcept -> bool override;
  auto operator!=(const Node& rhs) const noexcept -> bool override;

  [[nodiscard]] auto operator[](int i) const -> const Node& override;
  [[nodiscard]] auto getChildCount() const -> int override;

private:
  const lex::Token m_op;
  const NodePtr m_rhs;

  auto print(std::ostream& out) const -> std::ostream& override;
};

// Factories.
auto unaryExprNode(lex::Token op, NodePtr rhs) -> NodePtr;

} // namespace parse
