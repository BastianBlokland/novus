#include "prog/expr/node_assign.hpp"
#include "utilities.hpp"
#include <stdexcept>

namespace prog::expr {

AssignExprNode::AssignExprNode(sym::ConstId constId, NodePtr expr) :
    m_constId{constId}, m_expr{std::move(expr)} {}

auto AssignExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const AssignExprNode*>(&rhs);
  return r != nullptr && m_constId == r->m_constId && *m_expr == *r->m_expr;
}

auto AssignExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !AssignExprNode::operator==(rhs);
}

auto AssignExprNode::operator[](unsigned int i) const -> const Node& {
  if (i == 0) {
    return *m_expr;
  }
  throw std::out_of_range{"No child at given index"};
}

auto AssignExprNode::getChildCount() const -> unsigned int { return 1; }

auto AssignExprNode::getType() const noexcept -> sym::TypeId { return m_expr->getType(); }

auto AssignExprNode::print(std::ostream& out) const -> std::ostream& {
  return out << "assign-" << m_constId;
}

// Factories.
auto assignExprNode(const sym::ConstDeclTable& constTable, sym::ConstId constId, NodePtr expr)
    -> NodePtr {
  if (!expr) {
    throw std::invalid_argument{"Expression in assign node cannot be null"};
  }
  const auto constType = constTable[constId].getType();
  if (constType != expr->getType()) {
    throw std::invalid_argument{"Type of expression needs to match type of constant to assign"};
  }
  return std::unique_ptr<AssignExprNode>{new AssignExprNode{constId, std::move(expr)}};
}

} // namespace prog::expr
