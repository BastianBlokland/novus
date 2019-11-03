#include "prog/expr/node_group.hpp"
#include "utilities.hpp"
#include <stdexcept>

namespace prog::expr {

GroupExprNode::GroupExprNode(std::vector<NodePtr> exprs) : m_exprs{std::move(exprs)} {}

auto GroupExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const GroupExprNode*>(&rhs);
  return r != nullptr && nodesEqual(m_exprs, r->m_exprs);
}

auto GroupExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !GroupExprNode::operator==(rhs);
}

auto GroupExprNode::getType() const noexcept -> sym::TypeId { return m_exprs.back()->getType(); }

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
