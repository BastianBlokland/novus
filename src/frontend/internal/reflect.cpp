#include "internal/reflect.hpp"

namespace frontend::internal {

namespace {

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
