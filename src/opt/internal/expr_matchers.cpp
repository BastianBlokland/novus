#include "internal/expr_matchers.hpp"
#include "internal/find_called_funcs.hpp"
#include "prog/expr/node_visitor.hpp"
#include "prog/expr/nodes.hpp"
#include "prog/sym/delegate_def.hpp"
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

auto hasSideEffect(const prog::Program& prog, const prog::expr::Node& expr) -> bool {
  auto matcher = ExprMatcher{[&prog](const prog::expr::Node& n) {
    switch (n.getKind()) {
    case prog::expr::NodeKind::Call: {
      const auto* callNode = n.downcast<prog::expr::CallExprNode>();
      if (callNode->isLazy()) {
        // Lazy calls have no side-effects (as they do nothing until 'get' is called on the lazy
        // objects).
        return false;
      }
      // Normal or forked calls have side-effects if they are calling actions.
      return prog.getFuncDecl(callNode->getFunc()).isAction();
    }
    case prog::expr::NodeKind::CallDyn: {
      // Dynamic calls have side-effects when they are calling action delegates.
      const auto* callDynNode = n.downcast<prog::expr::CallDynExprNode>();
      auto delegateTypeId     = (*callDynNode)[0].getType();
      const auto& delegateDef = std::get<prog::sym::DelegateDef>(prog.getTypeDef(delegateTypeId));
      return delegateDef.isAction();
    }
    case prog::expr::NodeKind::Assign:
    case prog::expr::NodeKind::UnionGet:
      // Constant declarations are considered to have a side-effect as you cannot freely reorder.
      return true;
    default:
      return false;
    }
  }};

  expr.accept(&matcher);
  return matcher.isFound();
}

auto isLiteral(const prog::expr::Node& expr) -> bool {
  switch (expr.getKind()) {
  case prog::expr::NodeKind::LitBool:
  case prog::expr::NodeKind::LitChar:
  case prog::expr::NodeKind::LitEnum:
  case prog::expr::NodeKind::LitFloat:
  case prog::expr::NodeKind::LitFunc:
  case prog::expr::NodeKind::LitInt:
  case prog::expr::NodeKind::LitLong:
  case prog::expr::NodeKind::LitString:
    return true;
  default:
    return false;
  }
}

} // namespace opt::internal
