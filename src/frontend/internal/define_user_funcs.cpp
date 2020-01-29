#include "internal/define_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/check_inf_recursion.hpp"
#include "internal/const_binder.hpp"
#include "internal/get_expr.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include "prog/expr/node_call.hpp"
#include <cassert>

namespace frontend::internal {

template <typename FuncParseNode>
auto defineFunc(
    Context* ctx,
    const TypeSubstitutionTable* typeSubTable,
    prog::sym::FuncId id,
    std::string funcDisplayName,
    const FuncParseNode& n) -> bool {

  const auto& funcDecl   = ctx->getProg()->getFuncDecl(id);
  const auto funcRetType = funcDecl.getOutput();

  auto consts = prog::sym::ConstDeclTable{};
  if (!declareFuncInput(ctx, typeSubTable, n, &consts)) {
    return false;
  }

  auto allowActionCalls = funcDecl.isAction();
  auto visibleConsts    = consts.getAll();
  auto constBinder      = ConstBinder{&consts, &visibleConsts, nullptr};
  auto getExprFlags     = GetExpr::Flags::AllowPureFuncCalls;
  if (allowActionCalls) {
    getExprFlags = getExprFlags | GetExpr::Flags::AllowActionCalls;
  }

  auto getExpr = GetExpr{ctx, typeSubTable, &constBinder, funcRetType, getExprFlags};
  n[0].accept(&getExpr);
  auto expr = std::move(getExpr.getValue());

  // Report this diagnostic after processing the body so other errors have priority over this.
  if (funcRetType.isInfer()) {
    if (!ctx->hasErrors()) {
      ctx->reportDiag(errUnableToInferFuncReturnType, funcDisplayName, n.getSpan());
    }
    return false;
  }

  // Bail out if we failed to get an expression for the body.
  if (!expr) {
    assert(ctx->hasErrors());
    return false;
  }

  // Check if a pure functions contains infinite recursion.
  if (!funcDecl.isAction()) {
    auto checkInfRec = CheckInfRecursion{*ctx, id};
    expr->accept(&checkInfRec);
    if (checkInfRec.isInfRecursion()) {
      ctx->reportDiag(errPureFuncInfRecursion, n[0].getSpan());
      return false;
    }
  }

  if (expr->getType() == funcRetType) {
    ctx->getProg()->defineFunc(id, std::move(consts), std::move(expr));
    return true;
  }

  const auto conv = ctx->getProg()->lookupImplicitConv(expr->getType(), funcRetType);
  if (conv && *conv != id) {
    auto convArgs = std::vector<prog::expr::NodePtr>{};
    convArgs.push_back(std::move(expr));
    ctx->getProg()->defineFunc(
        id,
        std::move(consts),
        prog::expr::callExprNode(*ctx->getProg(), *conv, std::move(convArgs)));
    return true;
  }

  ctx->reportDiag(
      errNonMatchingFuncReturnType,
      funcDisplayName,
      getDisplayName(*ctx, funcRetType),
      getDisplayName(*ctx, expr->getType()),
      n[0].getSpan());
  return false;
}

// Explicit instantiations.
template bool defineFunc(
    Context* ctx,
    const TypeSubstitutionTable* typeSubTable,
    prog::sym::FuncId id,
    std::string funcDisplayName,
    const parse::FuncDeclStmtNode& n);

template bool defineFunc(
    Context* ctx,
    const TypeSubstitutionTable* typeSubTable,
    prog::sym::FuncId id,
    std::string funcDisplayName,
    const parse::AnonFuncExprNode& n);

} // namespace frontend::internal
