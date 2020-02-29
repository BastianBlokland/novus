#include "prog/expr/node_lit_long.hpp"
#include <stdexcept>

namespace prog::expr {

LitLongNode::LitLongNode(sym::TypeId type, int64_t val) : m_type{type}, m_val{val} {}

auto LitLongNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const LitLongNode*>(&rhs);
  return r != nullptr && m_val == r->m_val;
}

auto LitLongNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !LitLongNode::operator==(rhs);
}

auto LitLongNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto LitLongNode::getChildCount() const -> unsigned int { return 0; }

auto LitLongNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto LitLongNode::toString() const -> std::string { return std::to_string(m_val); }

auto LitLongNode::clone() const -> std::unique_ptr<Node> {
  return std::unique_ptr<LitLongNode>{new LitLongNode{m_type, m_val}};
}

auto LitLongNode::getVal() const noexcept -> int64_t { return m_val; }

auto LitLongNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto litLongNode(const Program& program, int64_t val) -> NodePtr {
  return std::unique_ptr<LitLongNode>{new LitLongNode{program.getLong(), val}};
}

} // namespace prog::expr
