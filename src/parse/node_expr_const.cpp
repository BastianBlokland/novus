#include "parse/node_expr_const.hpp"
#include "utilities.hpp"

namespace parse {

ConstExprNode::ConstExprNode(lex::Token id) : Node(NodeKind::ExprConst), m_id{std::move(id)} {}

auto ConstExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const ConstExprNode*>(&rhs);
  return r != nullptr && m_id == r->m_id;
}

auto ConstExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !ConstExprNode::operator==(rhs);
}

auto ConstExprNode::operator[](int /*unused*/) const -> const Node& {
  throw std::out_of_range("No child at given index");
}

auto ConstExprNode::getChildCount() const -> unsigned int { return 0; }

auto ConstExprNode::getSpan() const -> lex::SourceSpan { return m_id.getSpan(); }

auto ConstExprNode::getId() const -> const lex::Token& { return m_id; }

auto ConstExprNode::print(std::ostream& out) const -> std::ostream& {
  return out << ::parse::getId(m_id).value_or("error");
}

// Factories.
auto constExprNode(lex::Token id) -> NodePtr {
  return std::unique_ptr<ConstExprNode>{new ConstExprNode{std::move(id)}};
}

} // namespace parse
