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

} // namespace

auto reflectTypeKind(Context* ctx, prog::sym::TypeId type) noexcept -> prog::sym::TypeId {
  const auto& typeDecl = ctx->getProg()->getTypeDecl(type);
  return ctx->getStaticIntTable()->getType(ctx, static_cast<int32_t>(typeDecl.getKind()));
}

auto reflectStructFieldCount(Context* ctx, prog::sym::TypeId structType) noexcept
    -> std::optional<prog::sym::TypeId> {
  const auto* structDef = getStructDef(ctx, structType);
  if (!structDef) {
    return std::nullopt;
  }
  return ctx->getStaticIntTable()->getType(ctx, structDef->getFields().getCount());
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

auto reflectUnionCount(Context* ctx, prog::sym::TypeId unionType) noexcept
    -> std::optional<prog::sym::TypeId> {

  const auto* unionDef = getUnionDef(ctx, unionType);
  if (!unionDef) {
    return std::nullopt;
  }
  return ctx->getStaticIntTable()->getType(ctx, unionDef->getTypes().size());
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
  const auto* enumDef = getEnumDef(ctx, enumType);
  if (!enumDef) {
    return std::nullopt;
  }
  return ctx->getStaticIntTable()->getType(ctx, enumDef->getCount());
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

} // namespace frontend::internal
