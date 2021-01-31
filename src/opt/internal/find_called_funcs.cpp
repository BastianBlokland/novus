#include "find_called_funcs.hpp"
#include "prog/expr/nodes.hpp"

namespace opt::internal {

FindCalledFuncs::FindCalledFuncs(const prog::Program& prog, FuncSet* funcs) :
    m_prog{prog}, m_funcs{funcs} {
  if (m_funcs == nullptr) {
    throw std::invalid_argument{"Function set cannot be null"};
  }
}

auto FindCalledFuncs::visit(const prog::expr::CallExprNode& n) -> void {
  prog::expr::DeepNodeVisitor::visit(n);
  markFunc(n.getFunc());
}

auto FindCalledFuncs::markFunc(prog::sym::FuncId func) -> void {
  if (!m_funcs->insert(func).second) {
    return;
  }

  // Visit any newly found user functions.
  const auto& funcDecl = m_prog.getFuncDecl(func);
  if (funcDecl.getKind() == prog::sym::FuncKind::User) {
    const auto& funcDef = m_prog.getFuncDef(func);
    funcDef.getBody().accept(this);
  }
}

} // namespace opt::internal
