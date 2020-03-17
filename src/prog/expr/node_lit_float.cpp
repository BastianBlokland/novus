#include "prog/expr/node_lit_float.hpp"
#include <stdexcept>

namespace prog::expr {

LitFloatNode::LitFloatNode(sym::TypeId type, float val) :
    Node{LitFloatNode::getKind()}, m_type{type}, m_val{val} {}

auto LitFloatNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const LitFloatNode*>(&rhs);
  return r != nullptr && m_val == r->m_val;
}

auto LitFloatNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !LitFloatNode::operator==(rhs);
}

auto LitFloatNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto LitFloatNode::getChildCount() const -> unsigned int { return 0; }

auto LitFloatNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto LitFloatNode::toString() const -> std::string { return std::to_string(m_val); }

auto LitFloatNode::clone(Rewriter* /*rewriter*/) const -> std::unique_ptr<Node> {
  return std::unique_ptr<LitFloatNode>{new LitFloatNode{m_type, m_val}};
}

auto LitFloatNode::getVal() const noexcept -> float { return m_val; }

auto LitFloatNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto litFloatNode(const Program& program, float val) -> NodePtr {
  return std::unique_ptr<LitFloatNode>{new LitFloatNode{program.getFloat(), val}};
}

} // namespace prog::expr
