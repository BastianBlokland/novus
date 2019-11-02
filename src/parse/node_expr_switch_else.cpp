#include "parse/node_expr_switch_else.hpp"

namespace parse {

SwitchExprElseNode::SwitchExprElseNode(lex::Token kw, lex::Token arrow, NodePtr rhs) :
    Node(NodeKind::ExprSwitchElse),
    m_kw{std::move(kw)},
    m_arrow{std::move(arrow)},
    m_rhs{std::move(rhs)} {}

auto SwitchExprElseNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const SwitchExprElseNode*>(&rhs);
  return r != nullptr && *m_rhs == *r->m_rhs;
}

auto SwitchExprElseNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !SwitchExprElseNode::operator==(rhs);
}

auto SwitchExprElseNode::operator[](int i) const -> const Node& {
  if (i == 0) {
    return *m_rhs;
  }
  throw std::out_of_range("No child at given index");
}

auto SwitchExprElseNode::getChildCount() const -> unsigned int { return 1; }

auto SwitchExprElseNode::getSpan() const -> input::Span {
  return input::Span::combine(m_kw.getSpan(), m_rhs->getSpan());
}

auto SwitchExprElseNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto SwitchExprElseNode::print(std::ostream& out) const -> std::ostream& { return out << "else"; }

// Factories.
auto switchExprElseNode(lex::Token kw, lex::Token arrow, NodePtr rhs) -> NodePtr {
  if (rhs == nullptr) {
    throw std::invalid_argument("Rhs cannot be null");
  }
  return std::unique_ptr<SwitchExprElseNode>{
      new SwitchExprElseNode{std::move(kw), std::move(arrow), std::move(rhs)}};
}

} // namespace parse
