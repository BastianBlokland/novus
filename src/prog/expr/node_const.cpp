#include "prog/expr/node_const.hpp"

namespace prog::expr {

ConstExprNode::ConstExprNode(sym::ConstId id, sym::TypeId type) : m_id{id}, m_type{type} {}

auto ConstExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const ConstExprNode*>(&rhs);
  return r != nullptr && m_id == r->m_id;
}

auto ConstExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !ConstExprNode::operator==(rhs);
}

auto ConstExprNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto ConstExprNode::getChildCount() const -> unsigned int { return 0; }

auto ConstExprNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto ConstExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto ConstExprNode::print(std::ostream& out) const -> std::ostream& { return out << m_id; }

// Factories.
auto constExprNode(const sym::ConstDeclTable& constTable, sym::ConstId id) -> NodePtr {
  const auto type = constTable[id].getType();
  return std::unique_ptr<ConstExprNode>{new ConstExprNode{id, type}};
}

} // namespace prog::expr