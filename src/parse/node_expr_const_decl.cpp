#include "parse/node_expr_const_decl.hpp"

namespace parse {

ConstDeclExprNode::ConstDeclExprNode(lex::Token id, lex::Token eq, NodePtr rhs) :
    Node(NodeType::ExprConstDecl), m_id{std::move(id)}, m_eq{std::move(eq)}, m_rhs{std::move(rhs)} {

  if (m_rhs == nullptr) {
    throw std::invalid_argument("Rhs cannot be null");
  }
}

auto ConstDeclExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const ConstDeclExprNode*>(&rhs);
  return r != nullptr && m_id == r->m_id && *m_rhs == *r->m_rhs;
}

auto ConstDeclExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !ConstDeclExprNode::operator==(rhs);
}

auto ConstDeclExprNode::operator[](int i) const -> const Node& {
  if (i == 0) {
    return *m_rhs;
  }
  throw std::out_of_range("No child at given index");
}

auto ConstDeclExprNode::getChildCount() const -> unsigned int { return 1; }

auto ConstDeclExprNode::getSpan() const -> lex::SourceSpan {
  return lex::SourceSpan::combine(m_id.getSpan(), m_rhs->getSpan());
}

auto ConstDeclExprNode::getId() const -> const lex::Token& { return m_id; }

auto ConstDeclExprNode::print(std::ostream& out) const -> std::ostream& {
  return out << "decl-" << m_id;
}

// Factories.
auto constDeclExprNode(lex::Token id, lex::Token eq, NodePtr rhs) -> NodePtr {
  return std::make_unique<ConstDeclExprNode>(std::move(id), std::move(eq), std::move(rhs));
}

} // namespace parse
