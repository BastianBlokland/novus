#include "parse/node_expr_binary.hpp"

namespace parse {

BinaryExprNode::BinaryExprNode(NodePtr lhs, lex::Token op, NodePtr rhs) :
    Node(NodeKind::ExprBinaryOp),
    m_lhs{std::move(lhs)},
    m_op{std::move(op)},
    m_rhs{std::move(rhs)} {}

auto BinaryExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const BinaryExprNode*>(&rhs);
  return r != nullptr && *m_lhs == *r->m_lhs && m_op == r->m_op && *m_rhs == *r->m_rhs;
}

auto BinaryExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !BinaryExprNode::operator==(rhs);
}

auto BinaryExprNode::operator[](int i) const -> const Node& {
  switch (i) {
  case 0:
    return *m_lhs;
  case 1:
    return *m_rhs;
  default:
    throw std::out_of_range{"No child at given index"};
  }
}

auto BinaryExprNode::getChildCount() const -> unsigned int { return 2; }

auto BinaryExprNode::getSpan() const -> input::Span {
  return input::Span::combine(m_lhs->getSpan(), m_rhs->getSpan());
}

auto BinaryExprNode::getOperator() const -> const lex::Token& { return m_op; }

auto BinaryExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto BinaryExprNode::print(std::ostream& out) const -> std::ostream& { return out << m_op; }

// Factories.
auto binaryExprNode(NodePtr lhs, lex::Token op, NodePtr rhs) -> NodePtr {
  if (lhs == nullptr) {
    throw std::invalid_argument{"Lhs cannot be null"};
  }
  if (rhs == nullptr) {
    throw std::invalid_argument{"Rhs cannot be null"};
  }
  return std::unique_ptr<BinaryExprNode>{
      new BinaryExprNode{std::move(lhs), std::move(op), std::move(rhs)}};
}

} // namespace parse
