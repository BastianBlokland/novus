#include "prog/expr/node_lit_int.hpp"
#include <stdexcept>

namespace prog::expr {

LitIntNode::LitIntNode(sym::TypeId type, int32_t val) :
    Node{LitIntNode::getKind()}, m_type{type}, m_val{val} {}

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

auto LitIntNode::toString() const -> std::string { return std::to_string(m_val); }

auto LitIntNode::clone(Rewriter* /*rewriter*/) const -> std::unique_ptr<Node> {
  auto* newExpr = new LitIntNode{m_type, m_val};
  newExpr->setSourceId(getSourceId());
  return std::unique_ptr<LitIntNode>{newExpr};
}

auto LitIntNode::getVal() const noexcept -> int32_t { return m_val; }

auto LitIntNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto litIntNode(const Program& program, int32_t val) -> NodePtr {
  return std::unique_ptr<LitIntNode>{new LitIntNode{program.getInt(), val}};
}

} // namespace prog::expr
