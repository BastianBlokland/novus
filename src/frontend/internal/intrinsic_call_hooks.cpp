#include "internal/intrinsic_call_hooks.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/reflect.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_field.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"
#include <cassert>
#include <sstream>

namespace frontend::internal {

namespace {

using OptNodeExpr = std::optional<prog::expr::NodePtr>;

auto resolveTypeNameIntrinsic(
    Context* ctx, const prog::sym::TypeSet& typeParams, IntrinsicArgs& args) -> OptNodeExpr {

  if (typeParams.getCount() != 1 || args.first.size() != 0) {
    return std::nullopt;
  }
  return prog::expr::litStringNode(*ctx->getProg(), getDisplayName(*ctx, typeParams[0]));
}

auto resolveFailIntrinsic(
    Context* ctx, bool allowActions, const prog::sym::TypeSet& typeParams, IntrinsicArgs& args)
    -> OptNodeExpr {

  if (!allowActions || typeParams.getCount() != 1 || args.first.size() != 0) {
    return std::nullopt;
  }
  const auto funcId = ctx->getFails()->getFailIntrinsic(ctx, typeParams[0]);
  return prog::expr::callExprNode(*ctx->getProg(), funcId, {});
}

auto resolveStaticIntToIntIntrinsic(
    Context* ctx, const prog::sym::TypeSet& typeParams, IntrinsicArgs& args) -> OptNodeExpr {

  if (typeParams.getCount() != 1 || args.first.size() != 0) {
    return std::nullopt;
  }
  const auto value = ctx->getStaticIntTable()->getValue(typeParams[0]);
  return value ? prog::expr::litIntNode(*ctx->getProg(), *value) : OptNodeExpr{};
}

auto resolveReflectStructFieldName(
    Context* ctx, const prog::sym::TypeSet& typeParams, IntrinsicArgs& args) -> OptNodeExpr {

  if (typeParams.getCount() != 2 || args.first.size() != 0) {
    return std::nullopt;
  }
  auto name = reflectStructFieldName(ctx, typeParams[0], typeParams[1]);
  return name ? prog::expr::litStringNode(*ctx->getProg(), *name) : OptNodeExpr{};
}

auto resolveReflectStructField(
    Context* ctx, const prog::sym::TypeSet& typeParams, IntrinsicArgs& args) -> OptNodeExpr {

  if (typeParams.getCount() != 2 || args.first.size() != 1) {
    return std::nullopt;
  }
  const auto fieldId = reflectStructFieldId(ctx, typeParams[0], typeParams[1]);
  if (!fieldId) {
    return std::nullopt;
  }
  if (args.first[0]->getType() != typeParams[0]) {
    return std::nullopt; // Expression does not resolve to the expected type.
  }
  return prog::expr::fieldExprNode(*ctx->getProg(), std::move(args.first[0]), *fieldId);
}

auto resolveReflectEnumKey(Context* ctx, const prog::sym::TypeSet& typeParams, IntrinsicArgs& args)
    -> OptNodeExpr {

  if (typeParams.getCount() != 2 || args.first.size() != 0) {
    return std::nullopt;
  }
  auto key = reflectEnumKey(ctx, typeParams[0], typeParams[1]);
  return key ? prog::expr::litStringNode(*ctx->getProg(), *key) : OptNodeExpr{};
}

auto resolveReflectEnumValue(
    Context* ctx, const prog::sym::TypeSet& typeParams, IntrinsicArgs& args) -> OptNodeExpr {

  if (typeParams.getCount() != 2 || args.first.size() != 0) {
    return std::nullopt;
  }
  auto val = reflectEnumVal(ctx, typeParams[0], typeParams[1]);
  return val ? prog::expr::litIntNode(*ctx->getProg(), *val) : OptNodeExpr{};
}

auto resolveReflectDelegateIsAction(
    Context* ctx, const prog::sym::TypeSet& typeParams, IntrinsicArgs& args) -> OptNodeExpr {

  if (typeParams.getCount() != 1 || args.first.size() != 0) {
    return std::nullopt;
  }
  auto val = reflectDelegateIsAction(ctx, typeParams[0]);
  return val ? prog::expr::litBoolNode(*ctx->getProg(), *val) : OptNodeExpr{};
}

} // namespace

auto resolveMetaIntrinsic(
    Context* ctx,
    const TypeSubstitutionTable* subTable,
    bool allowActions,
    const lex::Token& nameToken,
    const std::optional<parse::TypeParamList>& typeParams,
    IntrinsicArgs& args) -> OptNodeExpr {

  assert(ctx);

  auto typeParamSet =
      typeParams ? getTypeSet(ctx, subTable, typeParams->getTypes()) : prog::sym::TypeSet{};
  if (!typeParamSet) {
    return std::nullopt;
  }

  const auto name = getName(nameToken);
  if (name == "type_name") {
    return resolveTypeNameIntrinsic(ctx, *typeParamSet, args);
  }
  if (name == "fail") {
    return resolveFailIntrinsic(ctx, allowActions, *typeParamSet, args);
  }
  if (name == "staticint_to_int") {
    return resolveStaticIntToIntIntrinsic(ctx, *typeParamSet, args);
  }
  if (name == "reflect_struct_field_name") {
    return resolveReflectStructFieldName(ctx, *typeParamSet, args);
  }
  if (name == "reflect_struct_field") {
    return resolveReflectStructField(ctx, *typeParamSet, args);
  }
  if (name == "reflect_enum_key") {
    return resolveReflectEnumKey(ctx, *typeParamSet, args);
  }
  if (name == "reflect_enum_value") {
    return resolveReflectEnumValue(ctx, *typeParamSet, args);
  }
  if (name == "reflect_delegate_is_action") {
    return resolveReflectDelegateIsAction(ctx, *typeParamSet, args);
  }
  return std::nullopt;
}

} // namespace frontend::internal
