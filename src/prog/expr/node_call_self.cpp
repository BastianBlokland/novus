#include "prog/expr/node_call_self.hpp"
#include "internal/implicit_conv.hpp"
#include "utilities.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

CallSelfExprNode::CallSelfExprNode(sym::TypeId resultType, std::vector<NodePtr> args) :
    m_resultType{resultType}, m_args{std::move(args)} {}

auto CallSelfExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const CallSelfExprNode*>(&rhs);
  return r != nullptr && m_resultType == r->m_resultType && nodesEqual(m_args, r->m_args);
}

auto CallSelfExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !CallSelfExprNode::operator==(rhs);
}

auto CallSelfExprNode::operator[](unsigned int i) const -> const Node& {
  if (i >= m_args.size()) {
    throw std::out_of_range("No child at given index");
  }
  return *m_args[i];
}

auto CallSelfExprNode::getChildCount() const -> unsigned int { return m_args.size(); }

auto CallSelfExprNode::getType() const noexcept -> sym::TypeId { return m_resultType; }

auto CallSelfExprNode::toString() const -> std::string { return "self-call"; }

auto CallSelfExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto callSelfExprNode(sym::TypeId resultType, std::vector<NodePtr> args) -> NodePtr {
  return std::unique_ptr<CallSelfExprNode>{new CallSelfExprNode{resultType, std::move(args)}};
}

} // namespace prog::expr
