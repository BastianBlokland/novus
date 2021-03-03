#include "frontend/diag_defs.hpp"
#include "internal/call_modifiers.hpp"
#include "internal/utilities.hpp"
#include <cassert>
#include <sstream>

namespace frontend::internal {

namespace {

auto injectFailIntrinsic(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    input::Span callSpan,
    const std::optional<parse::TypeParamList>& typeParams,
    const std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>& args,
    std::vector<prog::sym::FuncId>& possibleFuncs) -> void {

  if (!typeParams || typeParams->getCount() != 1 || args.first.size() != 0) {
    // Invalid fail intrinsic, expects 1 type parameter and 0 normal arguments.
    ctx->reportDiag(
        errInvalidFailIntrinsicCall,
        callSpan,
        typeParams ? typeParams->getCount() : 0u,
        args.first.size());
    return;
  }

  const auto resultType = getOrInstType(ctx, subTable, (*typeParams)[0]);
  possibleFuncs.push_back(ctx->getFails()->getFailIntrinsic(ctx, *resultType));
}

} // namespace

auto injectPossibleIntrinsicFunctions(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const lex::Token& nameToken,
    const input::Span callSpan,
    const std::optional<parse::TypeParamList>& typeParams,
    const std::pair<std::vector<prog::expr::NodePtr>, prog::sym::TypeSet>& args,
    std::vector<prog::sym::FuncId>& possibleFuncs) -> void {

  assert(ctx);

  if (getName(nameToken) == "fail") {
    injectFailIntrinsic(ctx, subTable, callSpan, typeParams, args, possibleFuncs);
  }
}

} // namespace frontend::internal
