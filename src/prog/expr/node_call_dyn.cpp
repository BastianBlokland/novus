#include "prog/expr/node_call_dyn.hpp"
#include "internal/implicit_conv.hpp"
#include "utilities.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

CallDynExprNode::CallDynExprNode(
    NodePtr lhs, sym::TypeId resultType, std::vector<NodePtr> args, bool fork) :
    m_lhs{std::move(lhs)}, m_resultType{resultType}, m_args{std::move(args)}, m_fork{fork} {}

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

auto CallDynExprNode::isFork() const noexcept -> bool { return m_fork; }

auto CallDynExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto callDynExprNode(const Program& prog, NodePtr lhs, std::vector<NodePtr> args) -> NodePtr {
  auto resultType = prog.getDelegateRetType(lhs->getType());
  if (!resultType) {
    throw std::invalid_argument{"Lhs expr to dyn call has to be a delegate type"};
  }
  return callDynExprNode(prog, std::move(lhs), *resultType, std::move(args), false);
}

auto callDynExprNode(
    const Program& prog, NodePtr lhs, sym::TypeId resultType, std::vector<NodePtr> args, bool fork)
    -> NodePtr {

  if (lhs == nullptr) {
    throw std::invalid_argument{"Lhs cannot be null"};
  }
  if (fork && !prog.isFuture(resultType)) {
    throw std::invalid_argument{"Forks have to return a future type"};
  }
  if (anyNodeNull(args)) {
    throw std::invalid_argument{"Call dyn node cannot contain a null argument"};
  }
  const auto& typeDecl = prog.getTypeDecl(lhs->getType());
  if (typeDecl.getKind() != sym::TypeKind::Delegate) {
    throw std::invalid_argument{"Lhs expr to dyn call has to be a delegate type"};
  }
  const auto& delegateDef   = std::get<sym::DelegateDef>(prog.getTypeDef(lhs->getType()));
  const auto& delegateInput = delegateDef.getInput();
  if (delegateInput.getCount() != args.size()) {
    throw std::invalid_argument{"Call node contains incorrect number of arguments"};
  }

  // Apply implicit conversions if necessary (and throw if types are incompatible).
  internal::applyImplicitConversions(prog, delegateInput, &args);

  return std::unique_ptr<CallDynExprNode>{
      new CallDynExprNode{std::move(lhs), resultType, std::move(args), fork}};
}

} // namespace prog::expr
