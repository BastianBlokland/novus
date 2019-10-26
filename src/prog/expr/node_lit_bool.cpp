#include "prog/expr/node_lit_bool.hpp"
#include <stdexcept>

namespace prog::expr {

LitBoolNode::LitBoolNode(sym::TypeId type, bool val) :
    Node{NodeKind::LitBool}, m_type{type}, m_val{val} {}

auto LitBoolNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const LitBoolNode*>(&rhs);
  return r != nullptr && m_val == r->m_val;
}

auto LitBoolNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !LitBoolNode::operator==(rhs);
}

auto LitBoolNode::getType() const noexcept -> sym::TypeId { return m_type; }

// Factories.
auto litBoolNode(const sym::TypeDeclTable& typeTable, bool val) -> NodePtr {
  const auto type = typeTable.lookup("bool");
  if (!type) {
    throw std::invalid_argument{"No 'bool' type present in type-table"};
  }
  return std::unique_ptr<LitBoolNode>{new LitBoolNode{type.value(), val}};
}

} // namespace prog::expr
