#include "parse/node_expr_switch.hpp"
#include "utilities.hpp"

namespace parse {

SwitchExprNode::SwitchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause) :
    m_ifClauses{std::move(ifClauses)}, m_elseClause{std::move(elseClause)} {}

auto SwitchExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const SwitchExprNode*>(&rhs);
  if (r == nullptr || !nodesEqual(m_ifClauses, r->m_ifClauses)) {
    return false;
  }
  if (hasElse() != r->hasElse()) {
    return false;
  }
  return m_elseClause == nullptr || *m_elseClause == *r->m_elseClause;
}

auto SwitchExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !SwitchExprNode::operator==(rhs);
}

auto SwitchExprNode::operator[](unsigned int i) const -> const Node& {
  if (i < m_ifClauses.size()) {
    return *m_ifClauses[i];
  }
  if (i == m_ifClauses.size() && hasElse()) {
    return *m_elseClause;
  }
  throw std::out_of_range{"No child at given index"};
}

auto SwitchExprNode::getChildCount() const -> unsigned int {
  return m_ifClauses.size() + (hasElse() ? 1 : 0);
}

auto SwitchExprNode::getSpan() const -> input::Span {
  return input::Span::combine(
      m_ifClauses.front()->getSpan(),
      hasElse() ? m_elseClause->getSpan() : m_ifClauses.back()->getSpan());
}

auto SwitchExprNode::hasElse() const -> bool { return m_elseClause != nullptr; }

auto SwitchExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto SwitchExprNode::print(std::ostream& out) const -> std::ostream& { return out << "switch"; }

// Factories.
auto switchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause) -> NodePtr {
  if (ifClauses.empty()) {
    throw std::invalid_argument{"Atleast one if clause is required"};
  }
  if (anyNodeNull(ifClauses)) {
    throw std::invalid_argument{"Switch cannot contain a null if-clause"};
  }
  return std::unique_ptr<SwitchExprNode>{
      new SwitchExprNode{std::move(ifClauses), std::move(elseClause)}};
}

} // namespace parse
