#include "internal/reflect.hpp"

namespace frontend::internal {

namespace {

auto getStructDef(Context* ctx, prog::sym::TypeId structType) noexcept
    -> const prog::sym::StructDef* {
  const auto& structTypeDecl = ctx->getProg()->getTypeDecl(structType);
  if (structTypeDecl.getKind() != prog::sym::TypeKind::Struct) {
    return nullptr; // Not a struct.
  }
  return &std::get<prog::sym::StructDef>(ctx->getProg()->getTypeDef(structType));
}

auto getUnionDef(Context* ctx, prog::sym::TypeId unionType) noexcept -> const prog::sym::UnionDef* {
  const auto& unionDecl = ctx->getProg()->getTypeDecl(unionType);
  if (unionDecl.getKind() != prog::sym::TypeKind::Union) {
    return nullptr; // Not a union.
  }
  return &std::get<prog::sym::UnionDef>(ctx->getProg()->getTypeDef(unionType));
}

auto getEnumDef(Context* ctx, prog::sym::TypeId enumType) noexcept -> const prog::sym::EnumDef* {
  const auto& enumTypeDecl = ctx->getProg()->getTypeDecl(enumType);
  if (enumTypeDecl.getKind() != prog::sym::TypeKind::Enum) {
    return nullptr; // Not an enum.
  }
  return &std::get<prog::sym::EnumDef>(ctx->getProg()->getTypeDef(enumType));
}

auto getDelegateDef(Context* ctx, prog::sym::TypeId delegateType) noexcept
    -> const prog::sym::DelegateDef* {

  const auto& delegateTypeDecl = ctx->getProg()->getTypeDecl(delegateType);
  if (delegateTypeDecl.getKind() != prog::sym::TypeKind::Delegate) {
    return nullptr; // Not a delegate.
  }
  return &std::get<prog::sym::DelegateDef>(ctx->getProg()->getTypeDef(delegateType));
}

} // namespace

auto reflectTypeKind(Context* ctx, prog::sym::TypeId type) noexcept -> prog::sym::TypeId {
  const auto& typeDecl = ctx->getProg()->getTypeDecl(type);
  return ctx->getStaticIntTable()->getType(ctx, static_cast<int32_t>(typeDecl.getKind()));
}

auto reflectStructFieldCount(Context* ctx, prog::sym::TypeId structType) noexcept
    -> std::optional<prog::sym::TypeId> {
  if (const auto* structDef = getStructDef(ctx, structType)) {
    return ctx->getStaticIntTable()->getType(ctx, structDef->getFields().getCount());
  }
  return std::nullopt;
}

auto reflectStructFieldName(
    Context* ctx, prog::sym::TypeId structType, prog::sym::TypeId indexType) noexcept
    -> std::optional<std::string> {

  const auto* structDef = getStructDef(ctx, structType);
  auto index            = ctx->getStaticIntTable()->getValue(indexType).value_or(-1);
  if (!structDef || index < 0 || index >= static_cast<int>(structDef->getFields().getCount())) {
    return std::nullopt; // Not a struct or out of bounds field index.
  }
  return structDef->getFields()[index].getName();
}

auto reflectStructFieldType(
    Context* ctx, prog::sym::TypeId structType, prog::sym::TypeId indexType) noexcept
    -> std::optional<prog::sym::TypeId> {

  const auto* structDef = getStructDef(ctx, structType);
  auto index            = ctx->getStaticIntTable()->getValue(indexType).value_or(-1);
  if (!structDef || index < 0 || index >= static_cast<int>(structDef->getFields().getCount())) {
    return std::nullopt; // Not a struct or out of bounds field index.
  }
  return structDef->getFields()[index].getType();
}

auto reflectStructFieldId(
    Context* ctx, prog::sym::TypeId structType, prog::sym::TypeId indexType) noexcept
    -> std::optional<prog::sym::FieldId> {

  const auto* structDef = getStructDef(ctx, structType);
  auto index            = ctx->getStaticIntTable()->getValue(indexType).value_or(-1);
  if (!structDef || index < 0 || index >= static_cast<int>(structDef->getFields().getCount())) {
    return std::nullopt; // Not a struct or out of bounds field index.
  }
  return structDef->getFields()[index].getId();
}

auto reflectUnionCount(Context* ctx, prog::sym::TypeId unionType) noexcept
    -> std::optional<prog::sym::TypeId> {

  if (const auto* unionDef = getUnionDef(ctx, unionType)) {
    return ctx->getStaticIntTable()->getType(ctx, unionDef->getTypes().size());
  }
  return std::nullopt;
}

auto reflectUnionType(
    Context* ctx, prog::sym::TypeId unionType, prog::sym::TypeId indexType) noexcept
    -> std::optional<prog::sym::TypeId> {

  const auto* unionDef = getUnionDef(ctx, unionType);
  auto index           = ctx->getStaticIntTable()->getValue(indexType).value_or(-1);
  if (!unionDef || index < 0 || index >= static_cast<int>(unionDef->getTypes().size())) {
    return std::nullopt; // Not a union or out of bounds index.
  }
  return unionDef->getTypes()[index];
}

auto reflectEnumCount(Context* ctx, prog::sym::TypeId enumType) noexcept
    -> std::optional<prog::sym::TypeId> {
  if (const auto* enumDef = getEnumDef(ctx, enumType)) {
    return ctx->getStaticIntTable()->getType(ctx, enumDef->getCount());
  }
  return std::nullopt;
}

auto reflectEnumKey(Context* ctx, prog::sym::TypeId enumType, prog::sym::TypeId indexType) noexcept
    -> std::optional<std::string> {

  const auto* enumDef = getEnumDef(ctx, enumType);

  auto index = ctx->getStaticIntTable()->getValue(indexType).value_or(-1);
  if (!enumDef || index < 0 || index >= static_cast<int>(enumDef->getCount())) {
    return std::nullopt; // Not an enum or out of bounds index.
  }
  return (*enumDef)[index].first;
}

auto reflectEnumVal(Context* ctx, prog::sym::TypeId enumType, prog::sym::TypeId indexType) noexcept
    -> std::optional<int32_t> {

  const auto* enumDef = getEnumDef(ctx, enumType);

  auto index = ctx->getStaticIntTable()->getValue(indexType).value_or(-1);
  if (!enumDef || index < 0 || index >= static_cast<int>(enumDef->getCount())) {
    return std::nullopt; // Not an enum or out of bounds index.
  }
  return (*enumDef)[index].second;
}

auto reflectDelegateInputCount(Context* ctx, prog::sym::TypeId delegateType) noexcept
    -> std::optional<prog::sym::TypeId> {
  if (const auto* delegateDef = getDelegateDef(ctx, delegateType)) {
    return ctx->getStaticIntTable()->getType(ctx, delegateDef->getInput().getCount());
  }
  return std::nullopt;
}

auto reflectDelegateInputType(
    Context* ctx, prog::sym::TypeId delegateType, prog::sym::TypeId indexType) noexcept
    -> std::optional<prog::sym::TypeId> {

  const auto* delegateDef = getDelegateDef(ctx, delegateType);
  auto index              = ctx->getStaticIntTable()->getValue(indexType).value_or(-1);
  if (!delegateDef || index < 0 || index >= static_cast<int>(delegateDef->getInput().getCount())) {
    return std::nullopt; // Not a delegate or out of bounds input index.
  }
  return delegateDef->getInput()[index];
}

auto reflectDelegateRetType(Context* ctx, prog::sym::TypeId delegateType) noexcept
    -> std::optional<prog::sym::TypeId> {
  if (const auto* delegateDef = getDelegateDef(ctx, delegateType)) {
    return delegateDef->getOutput();
  }
  return std::nullopt;
}

auto reflectDelegateIsAction(Context* ctx, prog::sym::TypeId delegateType) noexcept
    -> std::optional<bool> {

  if (const auto* delegateDef = getDelegateDef(ctx, delegateType)) {
    return delegateDef->isAction();
  }
  return std::nullopt;
}

} // namespace frontend::internal
