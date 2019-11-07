#include "prog/expr/node_lit_bool.hpp"
#include <stdexcept>

namespace prog::expr {

LitBoolNode::LitBoolNode(sym::TypeId type, bool val) : m_type{type}, m_val{val} {}

auto LitBoolNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const LitBoolNode*>(&rhs);
  return r != nullptr && m_val == r->m_val;
}

auto LitBoolNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !LitBoolNode::operator==(rhs);
}

auto LitBoolNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto LitBoolNode::getChildCount() const -> unsigned int { return 0; }

auto LitBoolNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto LitBoolNode::print(std::ostream& out) const -> std::ostream& {
  return out << (m_val ? "true" : "false");
}

// Factories.
auto litBoolNode(const Program& program, bool val) -> NodePtr {
  const auto type = program.lookupType("bool");
  if (!type) {
    throw std::invalid_argument{"No 'bool' type present in type-table"};
  }
  return std::unique_ptr<LitBoolNode>{new LitBoolNode{type.value(), val}};
}

} // namespace prog::expr
