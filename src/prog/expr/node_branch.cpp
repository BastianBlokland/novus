#include "prog/expr/node_branch.hpp"
#include "utilities.hpp"
#include <stdexcept>

namespace prog::expr {

BranchExprNode::BranchExprNode(std::vector<NodePtr> conditions, std::vector<NodePtr> branches) :
    m_conditions{std::move(conditions)}, m_branches{std::move(branches)} {}

auto BranchExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const BranchExprNode*>(&rhs);
  return r != nullptr && nodesEqual(m_conditions, r->m_conditions) &&
      nodesEqual(m_branches, r->m_branches);
}

auto BranchExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !BranchExprNode::operator==(rhs);
}

auto BranchExprNode::operator[](unsigned int i) const -> const Node& {
  if (i < m_conditions.size()) {
    return *m_conditions[i];
  }
  i -= m_conditions.size();
  if (i < m_branches.size()) {
    return *m_branches[i];
  }
  throw std::out_of_range{"No child at given index"};
}

auto BranchExprNode::getChildCount() const -> unsigned int {
  return m_conditions.size() + m_branches.size();
}

auto BranchExprNode::getType() const noexcept -> sym::TypeId {
  return m_branches.front()->getType();
}

auto BranchExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto BranchExprNode::print(std::ostream& out) const -> std::ostream& { return out << "branch"; }

// Factories.
auto branchExprNode(
    const Program& program, std::vector<NodePtr> conditions, std::vector<NodePtr> branches)
    -> NodePtr {

  if (conditions.empty()) {
    throw std::invalid_argument{"Branch node needs to contain atleast one condition"};
  }
  if (branches.size() != conditions.size() + 1) {
    throw std::invalid_argument{
        "Branch node needs to contain one more branch then conditions (else branch)"};
  }
  if (anyNodeNull(conditions)) {
    throw std::invalid_argument{"Branch node cannot contain a null condition"};
  }
  if (anyNodeNull(branches)) {
    throw std::invalid_argument{"Branch node cannot contain a null branch"};
  }
  const auto boolType = program.lookupType("bool");
  if (!boolType) {
    throw std::invalid_argument{"No 'bool' type present in type-table"};
  }
  if (getType(conditions) != boolType) {
    throw std::invalid_argument{"All conditions need to be of type 'bool'"};
  }
  if (!getType(branches)) {
    throw std::invalid_argument{"All branches need to have the same type"};
  }
  return std::unique_ptr<BranchExprNode>{
      new BranchExprNode{std::move(conditions), std::move(branches)}};
}

} // namespace prog::expr
