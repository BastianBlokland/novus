#include "prog/expr/node_call_dyn.hpp"
#include "internal/conversion.hpp"
#include "utilities.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

CallDynExprNode::CallDynExprNode(NodePtr lhs, sym::TypeId resultType, std::vector<NodePtr> args) :
    m_lhs{std::move(lhs)}, m_resultType{resultType}, m_args{std::move(args)} {}

auto CallDynExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const CallDynExprNode*>(&rhs);
  return r != nullptr && *m_lhs == *r->m_lhs && nodesEqual(m_args, r->m_args);
}

auto CallDynExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !CallDynExprNode::operator==(rhs);
}

auto CallDynExprNode::operator[](unsigned int i) const -> const Node& {
  if (i == 0) {
    return *m_lhs;
  }
  if (i > m_args.size()) {
    throw std::out_of_range{"No child at given index"};
  }
  return *m_args[i - 1];
}

auto CallDynExprNode::getChildCount() const -> unsigned int { return m_args.size() + 1; }

auto CallDynExprNode::getType() const noexcept -> sym::TypeId { return m_resultType; }

auto CallDynExprNode::toString() const -> std::string { return "call-dyn"; }

auto CallDynExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto callDynExprNode(const Program& prog, NodePtr lhs, std::vector<NodePtr> args) -> NodePtr {
  if (lhs == nullptr) {
    throw std::invalid_argument{"Lhs cannot be null"};
  }
  if (anyNodeNull(args)) {
    throw std::invalid_argument{"Call dyn node cannot contain a null argument"};
  }
  const auto& typeDecl = prog.getTypeDecl(lhs->getType());
  if (typeDecl.getKind() != sym::TypeKind::UserDelegate) {
    throw std::invalid_argument{"Lhs expr to dyn call has to be a delegate type"};
  }
  const auto& delegateDef   = std::get<sym::DelegateDef>(prog.getTypeDef(lhs->getType()));
  const auto& delegateInput = delegateDef.getInput();
  if (delegateInput.getCount() != args.size()) {
    throw std::invalid_argument{"Call node contains incorrect number of arguments"};
  }

  // Apply conversions if necessary (and throw if types are incompatible).
  internal::applyConversions(prog, delegateInput, &args);

  return std::unique_ptr<CallDynExprNode>{
      new CallDynExprNode{std::move(lhs), delegateDef.getOutput(), std::move(args)}};
}

} // namespace prog::expr
