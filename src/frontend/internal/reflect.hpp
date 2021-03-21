#pragma once
#include "internal/context.hpp"
#include <optional>
#include <string>

namespace frontend::internal {

[[nodiscard]] auto reflectTypeKind(Context* ctx, prog::sym::TypeId type) noexcept
    -> prog::sym::TypeId;

[[nodiscard]] auto reflectEnumCount(Context* ctx, prog::sym::TypeId enumType) noexcept
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto
reflectEnumKey(Context* ctx, prog::sym::TypeId enumType, prog::sym::TypeId indexType) noexcept
    -> std::optional<std::string>;

[[nodiscard]] auto
reflectEnumVal(Context* ctx, prog::sym::TypeId enumType, prog::sym::TypeId indexType) noexcept
    -> std::optional<int32_t>;

} // namespace frontend::internal
