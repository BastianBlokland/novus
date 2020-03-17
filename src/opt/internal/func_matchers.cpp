#include "internal/func_matchers.hpp"
#include "internal/find_called_funcs.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_visitor.hpp"
#include <unordered_set>

namespace opt::internal {

auto isUserFunc(const prog::Program& prog, prog::sym::FuncId funcId) -> bool {
  const auto& decl = prog.getFuncDecl(funcId);
  return decl.getKind() == prog::sym::FuncKind::User;
}

auto isRecursive(const prog::Program& prog, prog::sym::FuncId funcId) -> bool {
  const auto& funcDecl = prog.getFuncDecl(funcId);
  if (funcDecl.getKind() != prog::sym::FuncKind::User) {
    return false;
  }

  // Check if function contains a self-call.
  if (matchesExpr(prog, funcId, [](const prog::expr::Node& n) {
        return n.getKind() == prog::expr::NodeKind::CallSelf;
      })) {
    return true;
  }

  const auto& funcDef = prog.getFuncDef(funcId);

  // Gather all the functions this function (indirectly) calls.
  auto calledFuncs     = std::unordered_set<prog::sym::FuncId, prog::sym::FuncIdHasher>{};
  auto findCalledFuncs = FindCalledFuncs{prog, &calledFuncs};
  funcDef.getExpr().accept(&findCalledFuncs);

  // Check if this function ever (indirectly) calls itself.
  return calledFuncs.find(funcId) != calledFuncs.end();
}

} // namespace opt::internal
