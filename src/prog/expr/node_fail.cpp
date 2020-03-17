#include "prog/expr/node_fail.hpp"
#include <sstream>

namespace prog::expr {

FailNode::FailNode(sym::TypeId type) : Node{FailNode::getKind()}, m_type{type} {}

auto FailNode::operator==(const Node& rhs) const noexcept -> bool {
  return dynamic_cast<const FailNode*>(&rhs) != nullptr;
}

auto FailNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !FailNode::operator==(rhs);
}

auto FailNode::operator[](unsigned int /*unused*/) const -> const Node& {
  throw std::out_of_range{"No child at given index"};
}

auto FailNode::getChildCount() const -> unsigned int { return 0; }

auto FailNode::getType() const noexcept -> sym::TypeId { return m_type; }

auto FailNode::toString() const -> std::string { return "fail"; }

auto FailNode::clone(Rewriter* /*rewriter*/) const -> std::unique_ptr<Node> {
  return std::unique_ptr<FailNode>{new FailNode{m_type}};
}

auto FailNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto failNode(sym::TypeId type) -> NodePtr { return std::unique_ptr<FailNode>{new FailNode{type}}; }

} // namespace prog::expr
