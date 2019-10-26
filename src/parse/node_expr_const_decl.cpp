#include "parse/node_expr_const_decl.hpp"
#include "utilities.hpp"

namespace parse {

ConstDeclExprNode::ConstDeclExprNode(lex::Token id, lex::Token eq, NodePtr rhs) :
    Node(NodeKind::ExprConstDecl),
    m_id{std::move(id)},
    m_eq{std::move(eq)},
    m_rhs{std::move(rhs)} {}

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
  throw std::out_of_range{"No child at given index"};
}

auto ConstDeclExprNode::getChildCount() const -> unsigned int { return 1; }

auto ConstDeclExprNode::getSpan() const -> input::Span {
  return input::Span::combine(m_id.getSpan(), m_rhs->getSpan());
}

auto ConstDeclExprNode::getId() const -> const lex::Token& { return m_id; }

auto ConstDeclExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto ConstDeclExprNode::print(std::ostream& out) const -> std::ostream& {
  return out << "decl-" << ::parse::getId(m_id).value_or("error");
}

// Factories.
auto constDeclExprNode(lex::Token id, lex::Token eq, NodePtr rhs) -> NodePtr {
  if (rhs == nullptr) {
    throw std::invalid_argument{"Rhs cannot be null"};
  }
  return std::unique_ptr<ConstDeclExprNode>{
      new ConstDeclExprNode{std::move(id), std::move(eq), std::move(rhs)}};
}

} // namespace parse
