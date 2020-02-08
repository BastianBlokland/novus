#include "prog/expr/node_call.hpp"
#include "internal/implicit_conv.hpp"
#include "utilities.hpp"
#include <sstream>
#include <stdexcept>

namespace prog::expr {

CallExprNode::CallExprNode(
    sym::FuncId func, sym::TypeId resultType, std::vector<NodePtr> args, bool fork) :
    m_func{func}, m_resultType{resultType}, m_args{std::move(args)}, m_fork{fork} {}

auto CallExprNode::operator==(const Node& rhs) const noexcept -> bool {
  const auto r = dynamic_cast<const CallExprNode*>(&rhs);
  return r != nullptr && m_func == r->m_func && nodesEqual(m_args, r->m_args);
}

auto CallExprNode::operator!=(const Node& rhs) const noexcept -> bool {
  return !CallExprNode::operator==(rhs);
}

auto CallExprNode::operator[](unsigned int i) const -> const Node& {
  if (i >= m_args.size()) {
    throw std::out_of_range("No child at given index");
  }
  return *m_args[i];
}

auto CallExprNode::getChildCount() const -> unsigned int { return m_args.size(); }

auto CallExprNode::getType() const noexcept -> sym::TypeId { return m_resultType; }

auto CallExprNode::toString() const -> std::string {
  auto oss = std::ostringstream{};
  oss << "call-" << m_func;
  return oss.str();
}

auto CallExprNode::getFunc() const noexcept -> sym::FuncId { return m_func; }

auto CallExprNode::isFork() const noexcept -> bool { return m_fork; }

auto CallExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

// Factories.
auto callExprNode(const Program& prog, sym::FuncId func, std::vector<NodePtr> args) -> NodePtr {
  const auto& funcDecl = prog.getFuncDecl(func);
  return callExprNode(prog, func, funcDecl.getOutput(), std::move(args), false);
}

auto callExprNode(
    const Program& prog,
    sym::FuncId func,
    sym::TypeId resultType,
    std::vector<NodePtr> args,
    bool fork) -> NodePtr {

  if (anyNodeNull(args)) {
    throw std::invalid_argument{"Call node cannot contain a null argument"};
  }
  const auto& funcDecl = prog.getFuncDecl(func);
  if (fork && funcDecl.getKind() != sym::FuncKind::User) {
    throw std::invalid_argument{"Only user functions can be forked"};
  }
  if (fork && !prog.isFuture(resultType)) {
    throw std::invalid_argument{"Forks have to return a future type"};
  }

  const auto& funcInput = funcDecl.getInput();
  if (funcInput.getCount() != args.size()) {
    throw std::invalid_argument{"Call node contains incorrect number of arguments"};
  }

  // Apply implicit conversions if necessary (and throw if types are incompatible).
  internal::applyImplicitConversions(prog, funcDecl.getInput(), &args);

  return std::unique_ptr<CallExprNode>{
      new CallExprNode{funcDecl.getId(), resultType, std::move(args), fork}};
}

} // namespace prog::expr
