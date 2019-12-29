#include "parse/node_expr_id.hpp"
#include "utilities.hpp"

namespace parse {

IdExprNode::IdExprNode(lex::Token id) : m_id{std::move(id)} {}

auto IdExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const IdExprNode*>(&rhs);
  return r != nullptr && m_id == r->m_id;
}

auto IdExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !IdExprNode::operator==(rhs);
}

auto IdExprNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto IdExprNode::getChildCount() const -> unsigned int { return 0; }

auto IdExprNode::getSpan() const -> input::Span { return m_id.getSpan(); }

auto IdExprNode::getId() const -> const lex::Token& { return m_id; }

auto IdExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto IdExprNode::print(std::ostream& out) const -> std::ostream& {
  return out << ::parse::getId(m_id).value_or("error");
}

// Factories.
auto idExprNode(lex::Token id) -> NodePtr {
  return std::unique_ptr<IdExprNode>{new IdExprNode{std::move(id)}};
}

} // namespace parse
