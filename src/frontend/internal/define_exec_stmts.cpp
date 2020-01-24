#include "internal/define_exec_stmts.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/get_expr.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include "prog/expr/node_call.hpp"

namespace frontend::internal {

DefineExecStmts::DefineExecStmts(Context* ctx) : m_ctx{ctx} {
  if (m_ctx == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto DefineExecStmts::visit(const parse::ExecStmtNode& n) -> void {
  auto isValid       = true;
  auto consts        = prog::sym::ConstDeclTable{};
  auto visibleConsts = std::vector<prog::sym::ConstId>{};
  auto argTypes      = std::vector<prog::sym::TypeId>{};
  auto args          = std::vector<prog::expr::NodePtr>{};

  for (auto i = 0U; i < n.getChildCount(); ++i) {
    auto arg = getExpr(n[i], &consts, &visibleConsts, prog::sym::TypeId::inferType());
    if (arg) {
      argTypes.push_back(arg->getType());
      args.push_back(std::move(arg));
    } else {
      isValid = false;
    }
  }
  if (!isValid) {
    return;
  }

  const auto& targetName = getName(n.getTarget());
  const auto& target     = m_ctx->getProg()->lookupFunc(
      targetName, prog::sym::TypeSet{argTypes}, prog::OvOptions{prog::OvFlags::ExclPureFuncs});

  if (!target) {
    auto argTypeNames = std::vector<std::string>{};
    for (const auto& argType : argTypes) {
      argTypeNames.push_back(getDisplayName(*m_ctx, argType));
    }
    m_ctx->reportDiag(errInvalidExecStmt, targetName, argTypeNames, n.getSpan());
    return;
  }

  m_ctx->getProg()->addExecStmt(
      std::move(consts), prog::expr::callExprNode(*m_ctx->getProg(), *target, std::move(args)));
}

auto DefineExecStmts::getExpr(
    const parse::Node& n,
    prog::sym::ConstDeclTable* consts,
    std::vector<prog::sym::ConstId>* visibleConsts,
    prog::sym::TypeId typeHint) -> prog::expr::NodePtr {

  auto constBinder = ConstBinder{consts, visibleConsts, nullptr};
  auto getExpr = GetExpr{m_ctx, nullptr, &constBinder, typeHint, GetExpr::Flags::AllowActionCalls};
  n.accept(&getExpr);
  return std::move(getExpr.getValue());
}

} // namespace frontend::internal
