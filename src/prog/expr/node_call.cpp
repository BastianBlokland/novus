#include "prog/expr/node_call.hpp"
#include "utilities.hpp"
#include <stdexcept>

namespace prog::expr {

CallExprNode::CallExprNode(sym::FuncId func, sym::TypeId resultType, std::vector<NodePtr> args) :
    m_func{func}, m_resultType{resultType}, m_args{std::move(args)} {}

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

auto CallExprNode::getFunc() const noexcept -> sym::FuncId { return m_func; }

auto CallExprNode::accept(NodeVisitor* visitor) const -> void { visitor->visit(*this); }

auto CallExprNode::print(std::ostream& out) const -> std::ostream& {
  return out << "call-" << m_func;
}

// Factories.
auto callExprNode(const Program& program, sym::FuncId func, std::vector<NodePtr> args) -> NodePtr {
  if (anyNodeNull(args)) {
    throw std::invalid_argument{"Call node cannot contain a null argument"};
  }
  const auto& funcSig   = program.getFuncDecl(func).getSig();
  const auto& funcInput = funcSig.getInput();
  if (funcInput.getCount() != args.size()) {
    throw std::invalid_argument{"Call node contains incorrect number of arguments"};
  }
  for (auto i = 0U; i < funcInput.getCount(); ++i) {
    if (args[i]->getType() != funcInput[i]) {
      throw std::invalid_argument{
          "Call node contains argument who's type doesn't match function input"};
    }
  }
  return std::unique_ptr<CallExprNode>{
      new CallExprNode{func, funcSig.getOutput(), std::move(args)}};
}

} // namespace prog::expr
