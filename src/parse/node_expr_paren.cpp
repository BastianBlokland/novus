#include "parse/node_expr_paren.hpp"

namespace parse {

ParenExprNode::ParenExprNode(lex::Token open, NodePtr expr, lex::Token close) :
    Node(NodeType::ExprParen),
    m_open{std::move(open)},
    m_expr{std::move(expr)},
    m_close{std::move(close)} {

  if (m_expr == nullptr) {
    throw std::invalid_argument("Expr cannot be null");
  }
}

auto ParenExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const ParenExprNode*>(&rhs);
  return r != nullptr && m_open == r->m_open && *m_expr == *r->m_expr && m_close == r->m_close;
}

auto ParenExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !ParenExprNode::operator==(rhs);
}

auto ParenExprNode::operator[](int i) const -> const Node& {
  if (i == 0) {
    return *m_expr;
  }
  throw std::out_of_range("No child at given index");
}

auto ParenExprNode::getChildCount() const -> int { return 1; }

auto ParenExprNode::print(std::ostream& out) const -> std::ostream& {
  return out << "parenthesized";
}

// Factories.
auto parenExprNode(lex::Token open, NodePtr expr, lex::Token close) -> NodePtr {
  return std::make_unique<ParenExprNode>(std::move(open), std::move(expr), std::move(close));
}

} // namespace parse
