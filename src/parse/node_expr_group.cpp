#include "parse/node_expr_group.hpp"
#include "utilities.hpp"

namespace parse {

GroupExprNode::GroupExprNode(std::vector<NodePtr> subExprs, std::vector<lex::Token> semis) :
    m_subExprs{std::move(subExprs)}, m_semis{std::move(semis)} {}

auto GroupExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const GroupExprNode*>(&rhs);
  return r != nullptr && m_subExprs.size() == r->m_subExprs.size() &&
      std::equal(
             m_subExprs.begin(),
             m_subExprs.end(),
             r->m_subExprs.begin(),
             [](const NodePtr& l, const NodePtr& r) { return *l == *r; });
}

auto GroupExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !GroupExprNode::operator==(rhs);
}

auto GroupExprNode::operator[](unsigned int i) const -> const Node& {
  if (i >= m_subExprs.size()) {
    throw std::out_of_range{"No child at given index"};
  }
  return *m_subExprs[i];
}

auto GroupExprNode::getChildCount() const -> unsigned int { return m_subExprs.size(); }

auto GroupExprNode::getSpan() const -> input::Span {
  return input::Span::combine(m_subExprs.front()->getSpan(), m_subExprs.back()->getSpan());
}

auto GroupExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto GroupExprNode::print(std::ostream& out) const -> std::ostream& { return out << "group"; }

// Factories.
auto groupExprNode(std::vector<NodePtr> subExprs, std::vector<lex::Token> semis) -> NodePtr {
  if (subExprs.size() < 2) {
    throw std::invalid_argument{"Group expression has to contain aleast two sub-expressions"};
  }
  if (anyNodeNull(subExprs)) {
    throw std::invalid_argument{"subExprs cannot contain a nullptr"};
  }
  if (semis.size() != subExprs.size() - 1) {
    throw std::invalid_argument{"Incorrect number of semicolons"};
  }
  return std::unique_ptr<GroupExprNode>{new GroupExprNode{std::move(subExprs), std::move(semis)}};
}

} // namespace parse
