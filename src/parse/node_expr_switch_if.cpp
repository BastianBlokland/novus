#include "parse/node_expr_switch_if.hpp"

namespace parse {

SwitchExprIfNode::SwitchExprIfNode(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs) :
    Node(NodeType::ExprSwitchIf),
    m_kw{std::move(kw)},
    m_cond{std::move(cond)},
    m_arrow{std::move(arrow)},
    m_rhs{std::move(rhs)} {

  if (m_cond == nullptr) {
    throw std::invalid_argument("Condition expr cannot be null");
  }
  if (m_rhs == nullptr) {
    throw std::invalid_argument("Rhs cannot be null");
  }
}

auto SwitchExprIfNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const SwitchExprIfNode*>(&rhs);
  return r != nullptr && *m_cond == *r->m_cond && *m_rhs == *r->m_rhs;
}

auto SwitchExprIfNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !SwitchExprIfNode::operator==(rhs);
}

auto SwitchExprIfNode::operator[](int i) const -> const Node& {
  switch (i) {
  case 0:
    return *m_cond;
  case 1:
    return *m_rhs;
  default:
    throw std::out_of_range("No child at given index");
  }
}

auto SwitchExprIfNode::getChildCount() const -> unsigned int { return 2; }

auto SwitchExprIfNode::print(std::ostream& out) const -> std::ostream& { return out << "if"; }

// Factories.
auto switchExprIfNode(lex::Token kw, NodePtr cond, lex::Token arrow, NodePtr rhs) -> NodePtr {
  return std::make_unique<SwitchExprIfNode>(
      std::move(kw), std::move(cond), std::move(arrow), std::move(rhs));
}

} // namespace parse
