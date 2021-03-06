#include "internal/define_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/check_inf_recursion.hpp"
#include "internal/const_binder.hpp"
#include "internal/get_expr.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include "prog/expr/node_call.hpp"
#include <cassert>

namespace {

constexpr const size_t g_maxFuncDefinitionRecurseDepth = 500;

// Keep track of the function definition recursion depth, this is needed because with
// meta-programming you can actually build function definitions that recurse indefinitely.
// TODO: Implement this is a more elegant way.
thread_local size_t g_funcDefinitionRecurseDepth = 0;

struct RecursionTracker {
  RecursionTracker() { ++g_funcDefinitionRecurseDepth; }
  ~RecursionTracker() { --g_funcDefinitionRecurseDepth; }

  [[nodiscard]] auto isMaxFuncDefinitionRecurseDepthReached() const noexcept -> bool {
    return g_funcDefinitionRecurseDepth >= g_maxFuncDefinitionRecurseDepth;
  }
};

} // namespace

namespace frontend::internal {

template <typename FuncParseNode>
static auto getOptInputInitializers(
    Context* ctx,
    const TypeSubstitutionTable* typeSubTable,
    prog::sym::FuncId funcId,
    const FuncParseNode& n) -> std::vector<prog::expr::NodePtr> {

  const auto& funcDecl = ctx->getProg()->getFuncDecl(funcId);

  auto result = std::vector<prog::expr::NodePtr>{};
  result.reserve(funcDecl.getNumOptInputs());

  auto getExprFlags = GetExpr::Flags::AllowPureFuncCalls;
  if (funcDecl.isAction()) {
    getExprFlags = getExprFlags | GetExpr::Flags::AllowActionCalls;
  }

  const auto totalInputs       = funcDecl.getInput().getCount();
  const auto nonOptInputsCount = totalInputs - funcDecl.getNumOptInputs();
  for (auto i = 0u; i != funcDecl.getNumOptInputs(); ++i) {
    const auto type = funcDecl.getInput()[nonOptInputsCount + i];

    auto getExpr = GetExpr{ctx, typeSubTable, nullptr, type, std::nullopt, getExprFlags};
    n[i].accept(&getExpr);
    auto initializerExpr = std::move(getExpr.getValue());

    if (!initializerExpr) {
      assert(ctx->hasErrors());
      return {}; // Bail out if we failed to get an expression for the initializer.
    }

    if (initializerExpr->getType() == type) {
      result.push_back(std::move(initializerExpr));
      continue;
    }

    const auto conv = ctx->getProg()->lookupImplicitConv(initializerExpr->getType(), type);
    if (conv && *conv != funcId) {
      auto convArgs = std::vector<prog::expr::NodePtr>{};
      convArgs.push_back(std::move(initializerExpr));
      result.push_back(prog::expr::callExprNode(*ctx->getProg(), *conv, std::move(convArgs)));
      continue;
    }

    ctx->reportDiag(
        errNonMatchingInitializerType,
        n[i].getSpan(),
        getDisplayName(*ctx, type),
        getDisplayName(*ctx, initializerExpr->getType()));
  }

  return result;
}

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

  auto recursionTracker = RecursionTracker{};
  if (recursionTracker.isMaxFuncDefinitionRecurseDepthReached()) {
    ctx->reportDiag(errTooDeepRecursionInFunctionBody, n.getBody().getSpan(), funcDisplayName);
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
      ctx->reportDiag(errUnableToInferFuncReturnType, n.getSpan(), funcDisplayName);
    }
    return false;
  }

  if (!bodyExpr) {
    return false; // Bail out if we failed to get an expression for the body.
  }

  // Check if a pure functions contains infinite recursion.
  if (!funcDecl.isAction()) {
    auto checkInfRec = CheckInfRecursion{*ctx, id};
    bodyExpr->accept(&checkInfRec);
    if (checkInfRec.isInfRecursion()) {
      ctx->reportDiag(errPureFuncInfRecursion, n.getBody().getSpan());
      return false;
    }
  }

  // Define the initializers for the optional arguments.
  auto optInputInitializers = getOptInputInitializers(ctx, typeSubTable, id, n);
  if (optInputInitializers.size() != funcDecl.getNumOptInputs()) {
    assert(ctx->hasErrors());
    return false;
  }

  const auto isNoInline = n.hasModifier(lex::Keyword::Noinline);
  const auto funcDefFlags =
      isNoInline ? prog::sym::FuncDef::Flags::NoInline : prog::sym::FuncDef::Flags::None;

  if (bodyExpr->getType() == funcRetType) {
    ctx->getProg()->defineFunc(
        id, std::move(consts), std::move(bodyExpr), std::move(optInputInitializers), funcDefFlags);
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
        std::move(optInputInitializers),
        funcDefFlags);
    return true;
  }

  ctx->reportDiag(
      errNonMatchingFuncReturnType,
      n.getBody().getSpan(),
      funcDisplayName,
      getDisplayName(*ctx, funcRetType),
      getDisplayName(*ctx, bodyExpr->getType()));
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
