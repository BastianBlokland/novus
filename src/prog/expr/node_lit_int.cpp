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

auto LitIntNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto LitIntNode::getChildCount() const -> unsigned int { return 0; }

auto LitIntNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto LitIntNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto LitIntNode::print(std::ostream& out) const -> std::ostream& { return out << m_val; }

// Factories.
auto litIntNode(const Program& program, int32_t val) -> NodePtr {
  const auto type = program.lookupType("int");
  if (!type) {
    throw std::invalid_argument{"No 'int' type present in type-table"};
  }
  return std::unique_ptr<LitIntNode>{new LitIntNode{type.value(), val}};
}

} // namespace prog::expr
