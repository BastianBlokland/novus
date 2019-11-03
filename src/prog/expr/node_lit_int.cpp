#include "prog/expr/node_lit_int.hpp"
#include <stdexcept>

namespace prog::expr {

LitIntNode::LitIntNode(sym::TypeId type, int32_t val) : m_type{type}, m_val{val} {}

auto LitIntNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const LitIntNode*>(&rhs);
  return r != nullptr && m_val == r->m_val;
}

auto LitIntNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !LitIntNode::operator==(rhs);
}

auto LitIntNode::getType() const noexcept -> sym::TypeId { return m_type; }

// Factories.
auto litIntNode(const sym::TypeDeclTable& typeTable, int32_t val) -> NodePtr {
  const auto type = typeTable.lookup("int");
  if (!type) {
    throw std::invalid_argument{"No 'int' type present in type-table"};
  }
  return std::unique_ptr<LitIntNode>{new LitIntNode{type.value(), val}};
}

} // namespace prog::expr
