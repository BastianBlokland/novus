#include "parse/node_expr_unary.hpp"

namespace parse {

UnaryExprNode::UnaryExprNode(lex::Token op, NodePtr rhs) :
    Node(NodeType::ExprUnaryOp), m_op{std::move(op)}, m_rhs{std::move(rhs)} {

  if (m_rhs == nullptr) {
    throw std::invalid_argument("Rhs cannot be null");
  }
}

auto UnaryExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const UnaryExprNode*>(&rhs);
  return r != nullptr && m_op == r->m_op && *m_rhs == *r->m_rhs;
}

auto UnaryExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !UnaryExprNode::operator==(rhs);
}

[[nodiscard]] auto UnaryExprNode::operator[](int i) const -> const Node& {
  if (i == 0) {
    return *m_rhs;
  }
  throw std::out_of_range("No child at given index");
}

auto UnaryExprNode::getChildCount() const -> unsigned int { return 1; }

auto UnaryExprNode::getSpan() const -> lex::SourceSpan {
  return lex::SourceSpan::combine(m_op.getSpan(), m_rhs->getSpan());
}

auto UnaryExprNode::getOperator() const -> const lex::Token& { return m_op; }

auto UnaryExprNode::print(std::ostream& out) const -> std::ostream& { return out << m_op; }

// Factories.
auto unaryExprNode(lex::Token op, NodePtr rhs) -> NodePtr {
  return std::make_unique<UnaryExprNode>(std::move(op), std::move(rhs));
}

} // namespace parse
