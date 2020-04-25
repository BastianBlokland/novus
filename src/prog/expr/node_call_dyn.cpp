#include "prog/expr/node_call_dyn.hpp"
#include "internal/implicit_conv.hpp"
#include "prog/expr/rewriter.hpp"
#include "utilities.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

CallDynExprNode::CallDynExprNode(
    NodePtr lhs, sym::TypeId resultType, std::vector<NodePtr> args, CallMode mode) :
    Node{CallDynExprNode::getKind()},
    m_lhs{std::move(lhs)},
    m_resultType{resultType},
    m_args{std::move(args)},
    m_mode{mode} {}

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

auto CallDynExprNode::clone(Rewriter* rewriter) const -> std::unique_ptr<Node> {
  return std::unique_ptr<CallDynExprNode>{
      new CallDynExprNode{rewriter ? rewriter->rewrite(*m_lhs) : m_lhs->clone(nullptr),
                          m_resultType,
                          cloneNodes(m_args, rewriter),
                          m_mode}};
}

auto CallDynExprNode::getArgs() const noexcept -> const std::vector<NodePtr>& { return m_args; }

auto CallDynExprNode::getMode() const noexcept -> CallMode { return m_mode; }

auto CallDynExprNode::isFork() const noexcept -> bool { return m_mode == CallMode::Forked; }

auto CallDynExprNode::isLazy() const noexcept -> bool { return m_mode == CallMode::Lazy; }

auto CallDynExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto callDynExprNode(const Program& prog, NodePtr lhs, std::vector<NodePtr> args) -> NodePtr {
  auto resultType = prog.getDelegateRetType(lhs->getType());
  if (!resultType) {
    throw std::invalid_argument{"Lhs expr to dyn call has to be a delegate type"};
  }
  return callDynExprNode(prog, std::move(lhs), *resultType, std::move(args), CallMode::Normal);
}

auto callDynExprNode(
    const Program& prog,
    NodePtr lhs,
    sym::TypeId resultType,
    std::vector<NodePtr> args,
    CallMode mode) -> NodePtr {

  if (lhs == nullptr) {
    throw std::invalid_argument{"Lhs cannot be null"};
  }

  // Validate mode.
  switch (mode) {
  case CallMode::Forked:
    if (!prog.isFuture(resultType)) {
      throw std::invalid_argument{"Forks have to return a future type"};
    }
    break;
  case CallMode::Lazy:
    if (!prog.isLazy(resultType)) {
      throw std::invalid_argument{"Lazy calls have to return a lazy type"};
    }
  case CallMode::Normal:
    break;
  }

  // Validate arguments.
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

  if (mode == CallMode::Lazy && delegateDef.isAction()) {
    throw std::invalid_argument{"Lazy calls cannot be made to actions (impure)"};
  }

  // Apply implicit conversions if necessary (and throw if types are incompatible).
  internal::applyImplicitConversions(prog, delegateInput, &args);

  return std::unique_ptr<CallDynExprNode>{
      new CallDynExprNode{std::move(lhs), resultType, std::move(args), mode}};
}

} // namespace prog::expr
