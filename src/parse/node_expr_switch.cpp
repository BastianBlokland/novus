#include "parse/node_expr_switch.hpp"
#include "utilities.hpp"

namespace parse {

SwitchExprNode::SwitchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause) :
    Node(NodeType::ExprSwitch),
    m_ifClauses{std::move(ifClauses)},
    m_elseClause{std::move(elseClause)} {}

auto SwitchExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const SwitchExprNode*>(&rhs);
  return r != nullptr && nodesEqual(m_ifClauses, r->m_ifClauses) &&
      *m_elseClause == *r->m_elseClause;
}

auto SwitchExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !SwitchExprNode::operator==(rhs);
}

auto SwitchExprNode::operator[](int i) const -> const Node& {
  auto index = static_cast<unsigned>(i);
  if (i >= 0 && index < m_ifClauses.size()) {
    return *m_ifClauses[index];
  }
  if (index == m_ifClauses.size()) {
    return *m_elseClause;
  }
  throw std::out_of_range("No child at given index");
}

auto SwitchExprNode::getChildCount() const -> unsigned int { return m_ifClauses.size() + 1; }

auto SwitchExprNode::getSpan() const -> lex::SourceSpan {
  return lex::SourceSpan::combine(m_ifClauses.front()->getSpan(), m_elseClause->getSpan());
}

auto SwitchExprNode::print(std::ostream& out) const -> std::ostream& { return out << "switch"; }

// Factories.
auto switchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause) -> NodePtr {
  if (ifClauses.empty()) {
    throw std::invalid_argument("Atleast one if clause is required");
  }
  if (anyNodeNull(ifClauses)) {
    throw std::invalid_argument("Switch cannot contain a null if-clause");
  }
  if (elseClause == nullptr) {
    throw std::invalid_argument("Else-clause cannot be null");
  }
  return std::unique_ptr<SwitchExprNode>{
      new SwitchExprNode{std::move(ifClauses), std::move(elseClause)}};
}

} // namespace parse
