#include "prog/expr/node_group.hpp"
#include "utilities.hpp"
#include <stdexcept>

namespace prog::expr {

GroupExprNode::GroupExprNode(std::vector<NodePtr> exprs) :
    Node{GroupExprNode::getKind()}, m_exprs{std::move(exprs)} {}

auto GroupExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const GroupExprNode*>(&rhs);
  return r != nullptr && nodesEqual(m_exprs, r->m_exprs);
}

auto GroupExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !GroupExprNode::operator==(rhs);
}

auto GroupExprNode::operator[](unsigned int i) const -> const Node& {
  if (i >= m_exprs.size()) {
    throw std::out_of_range("No child at given index");
  }
  return *m_exprs[i];
}

auto GroupExprNode::getChildCount() const -> unsigned int { return m_exprs.size(); }

auto GroupExprNode::getType() const noexcept -> sym::TypeId { return m_exprs.back()->getType(); }

auto GroupExprNode::toString() const -> std::string { return "group"; }

auto GroupExprNode::clone(Rewriter* rewriter) const -> std::unique_ptr<Node> {
  return std::unique_ptr<GroupExprNode>{new GroupExprNode{cloneNodes(m_exprs, rewriter)}};
}

auto GroupExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto groupExprNode(std::vector<NodePtr> exprs) -> NodePtr {
  if (exprs.size() < 2) {
    throw std::invalid_argument{"Group node needs to contain atleast two expressions"};
  }
  if (anyNodeNull(exprs)) {
    throw std::invalid_argument{"Group node cannot contain a null expression"};
  }
  return std::unique_ptr<GroupExprNode>{new GroupExprNode{std::move(exprs)}};
}

} // namespace prog::expr
