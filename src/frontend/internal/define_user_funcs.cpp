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
  auto funcSignature     = std::make_pair(funcDecl.getInput(), funcRetType);

  auto consts = prog::sym::ConstDeclTable{};
  if (!declareFuncInput(ctx, typeSubTable, n, &consts, true)) {
    return false;
  }

  auto allowActionCalls = funcDecl.isAction();
  auto visibleConsts    = consts.getAll();
  auto constBinder      = ConstBinder{&consts, &visibleConsts, nullptr};
  auto getExprFlags     = GetExpr::Flags::AllowPureFuncCalls;
  if (allowActionCalls) {
    getExprFlags = getExprFlags | GetExpr::Flags::AllowActionCalls;
  }

  auto getExprBody =
      GetExpr{ctx, typeSubTable, &constBinder, funcRetType, funcSignature, getExprFlags};
  n.getBody().accept(&getExprBody);
  auto bodyExpr = std::move(getExprBody.getValue());

  // Report this diagnostic after processing the body so other errors have priority over this.
  if (funcRetType.isInfer()) {
    if (!ctx->hasErrors()) {
      ctx->reportDiag(errUnableToInferFuncReturnType, funcDisplayName, n.getSpan());
    }
    return false;
  }

  if (!bodyExpr) {
    return false; // Bail out if we failed to get an expression for the body.
  }

  // TODO: Define optional argument intializers.
  auto optArgInitializers = std::vector<prog::expr::NodePtr>{};

  // Check if a pure functions contains infinite recursion.
  if (!funcDecl.isAction()) {
    auto checkInfRec = CheckInfRecursion{*ctx, id};
    bodyExpr->accept(&checkInfRec);
    if (checkInfRec.isInfRecursion()) {
      ctx->reportDiag(errPureFuncInfRecursion, n.getBody().getSpan());
      return false;
    }
  }

  if (bodyExpr->getType() == funcRetType) {
    ctx->getProg()->defineFunc(
        id, std::move(consts), std::move(bodyExpr), std::move(optArgInitializers));
    return true;
  }

  const auto conv = ctx->getProg()->lookupImplicitConv(bodyExpr->getType(), funcRetType);
  if (conv && *conv != id) {
    auto convArgs = std::vector<prog::expr::NodePtr>{};
    convArgs.push_back(std::move(bodyExpr));
    ctx->getProg()->defineFunc(
        id,
        std::move(consts),
        prog::expr::callExprNode(*ctx->getProg(), *conv, std::move(convArgs)),
        std::move(optArgInitializers));
    return true;
  }

  ctx->reportDiag(
      errNonMatchingFuncReturnType,
      funcDisplayName,
      getDisplayName(*ctx, funcRetType),
      getDisplayName(*ctx, bodyExpr->getType()),
      n.getBody().getSpan());
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
