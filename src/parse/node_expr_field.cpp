#include "parse/node_expr_field.hpp"
#include "utilities.hpp"

namespace parse {

FieldExprNode::FieldExprNode(NodePtr lhs, lex::Token dot, lex::Token id) :
    m_lhs{std::move(lhs)}, m_dot{std::move(dot)}, m_id{std::move(id)} {}

auto FieldExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const FieldExprNode*>(&rhs);
  return r != nullptr && *m_lhs == *r->m_lhs && m_id == r->m_id;
}

auto FieldExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !FieldExprNode::operator==(rhs);
}

auto FieldExprNode::operator[](unsigned int i) const -> const Node& {
  if (i == 0) {
    return *m_lhs;
  }
  throw std::out_of_range{"No child at given index"};
}

auto FieldExprNode::getChildCount() const -> unsigned int { return 1; }

auto FieldExprNode::getSpan() const -> input::Span {
  return input::Span::combine(m_lhs->getSpan(), m_id.getSpan());
}

auto FieldExprNode::getId() const -> const lex::Token& { return m_id; }

auto FieldExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto FieldExprNode::print(std::ostream& out) const -> std::ostream& {
  return out << '.' << parse::getId(m_id).value_or("error");
}

// Factories.
auto fieldExprNode(NodePtr lhs, lex::Token dot, lex::Token id) -> NodePtr {
  if (lhs == nullptr) {
    throw std::invalid_argument{"Lhs cannot be null"};
  }
  return std::unique_ptr<FieldExprNode>{
      new FieldExprNode{std::move(lhs), std::move(dot), std::move(id)}};
}

} // namespace parse
