#include "internal/intrinsic_call_hooks.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"
#include <cassert>
#include <sstream>

namespace frontend::internal {

namespace {

using OptNodeExpr = std::optional<prog::expr::NodePtr>;

auto resolveTypeNameIntrinsic(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const std::optional<parse::TypeParamList>& typeParams,
    const IntrinsicArgs& args) -> OptNodeExpr {

  if (!typeParams || typeParams->getCount() != 1 || args.first.size() != 0) {
    return std::nullopt;
  }
  const auto type = getOrInstType(ctx, subTable, (*typeParams)[0]);
  if (!type) {
    return std::nullopt;
  }
  return prog::expr::litStringNode(*ctx->getProg(), getDisplayName(*ctx, *type));
}

auto resolveFailIntrinsic(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    bool allowActions,
    const std::optional<parse::TypeParamList>& typeParams,
    const IntrinsicArgs& args) -> OptNodeExpr {

  if (!allowActions || !typeParams || typeParams->getCount() != 1 || args.first.size() != 0) {
    return std::nullopt;
  }
  const auto resultType = getOrInstType(ctx, subTable, (*typeParams)[0]);
  if (!resultType) {
    return std::nullopt;
  }
  const auto funcId = ctx->getFails()->getFailIntrinsic(ctx, *resultType);
  return prog::expr::callExprNode(*ctx->getProg(), funcId, {});
}

auto resolveStaticIntToIntIntrinsic(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    const std::optional<parse::TypeParamList>& typeParams,
    const IntrinsicArgs& args) -> OptNodeExpr {

  if (!typeParams || typeParams->getCount() != 1 || args.first.size() != 0) {
    return std::nullopt;
  }
  const auto type = getOrInstType(ctx, subTable, (*typeParams)[0]);
  if (!type) {
    return std::nullopt;
  }
  return prog::expr::litIntNode(
      *ctx->getProg(), ctx->getStaticIntTable()->getValue(*type).value_or(-1));
}

} // namespace

auto resolveMetaIntrinsic(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    bool allowActions,
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    const IntrinsicArgs& args) -> OptNodeExpr {

  assert(ctx);

  const auto name = getName(nameToken);
  if (name == "type_name") {
    return resolveTypeNameIntrinsic(ctx, subTable, typeParams, args);
  }
  if (name == "fail") {
    return resolveFailIntrinsic(ctx, subTable, allowActions, typeParams, args);
  }
  if (name == "staticint_to_int") {
    return resolveStaticIntToIntIntrinsic(ctx, subTable, typeParams, args);
  }
  return std::nullopt;
}

} // namespace frontend::internal
