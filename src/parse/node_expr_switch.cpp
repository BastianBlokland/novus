#include "parse/node_expr_switch.hpp"

namespace parse {

SwitchExprNode::SwitchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause) :
    Node(NodeType::ExprSwitch),
    m_ifClauses{std::move(ifClauses)},
    m_elseClause{std::move(elseClause)} {

  if (m_ifClauses.empty()) {
    throw std::invalid_argument("Atleast one if clause is required");
  }
  if (std::any_of(
          m_ifClauses.begin(), m_ifClauses.end(), [](const NodePtr& p) { return p == nullptr; })) {
    throw std::invalid_argument("Switch cannot contain a null if-clause");
  }
  if (m_elseClause == nullptr) {
    throw std::invalid_argument("Else-clause cannot be null");
  }
}

auto SwitchExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const SwitchExprNode*>(&rhs);
  return r != nullptr && m_ifClauses.size() == r->m_ifClauses.size() &&
      std::equal(
             m_ifClauses.begin(),
             m_ifClauses.end(),
             r->m_ifClauses.begin(),
             [](const NodePtr& l, const NodePtr& r) { return *l == *r; }) &&
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

auto SwitchExprNode::print(std::ostream& out) const -> std::ostream& { return out << "switch"; }

// Factories.
auto switchExprNode(std::vector<NodePtr> ifClauses, NodePtr elseClause) -> NodePtr {
  return std::make_unique<SwitchExprNode>(std::move(ifClauses), std::move(elseClause));
}

} // namespace parse
