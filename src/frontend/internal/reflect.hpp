#pragma once
#include "internal/context.hpp"
#include <optional>
#include <string>

namespace frontend::internal {

[[nodiscard]] auto reflectTypeKind(Context* ctx, prog::sym::TypeId type) noexcept
    -> prog::sym::TypeId;

[[nodiscard]] auto reflectStructFieldCount(Context* ctx, prog::sym::TypeId structType) noexcept
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto reflectStructFieldName(
    Context* ctx, prog::sym::TypeId structType, prog::sym::TypeId indexType) noexcept
    -> std::optional<std::string>;

[[nodiscard]] auto reflectStructFieldType(
    Context* ctx, prog::sym::TypeId structType, prog::sym::TypeId indexType) noexcept
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto reflectStructFieldId(
    Context* ctx, prog::sym::TypeId structType, prog::sym::TypeId indexType) noexcept
    -> std::optional<prog::sym::FieldId>;

[[nodiscard]] auto reflectUnionCount(Context* ctx, prog::sym::TypeId unionType) noexcept
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto
reflectUnionType(Context* ctx, prog::sym::TypeId unionType, prog::sym::TypeId indexType) noexcept
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto reflectEnumCount(Context* ctx, prog::sym::TypeId enumType) noexcept
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto
reflectEnumKey(Context* ctx, prog::sym::TypeId enumType, prog::sym::TypeId indexType) noexcept
    -> std::optional<std::string>;

[[nodiscard]] auto
reflectEnumVal(Context* ctx, prog::sym::TypeId enumType, prog::sym::TypeId indexType) noexcept
    -> std::optional<int32_t>;

[[nodiscard]] auto reflectDelegateInputCount(Context* ctx, prog::sym::TypeId delegateType) noexcept
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto reflectDelegateInputType(
    Context* ctx, prog::sym::TypeId delegateType, prog::sym::TypeId indexType) noexcept
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto reflectDelegateRetType(Context* ctx, prog::sym::TypeId delegateType) noexcept
    -> std::optional<prog::sym::TypeId>;

[[nodiscard]] auto reflectDelegateIsAction(Context* ctx, prog::sym::TypeId delegateType) noexcept
    -> std::optional<bool>;

[[nodiscard]] auto reflectUsertypesAreEquivalentIntrinsic(
    Context* ctx, prog::sym::TypeId typeX, prog::sym::TypeId typeY) noexcept -> prog::sym::TypeId;

[[nodiscard]] auto reflectUsertypeAliasIntrinsic(
    Context* ctx, prog::sym::TypeId input, prog::sym::TypeId output) noexcept
    -> std::optional<prog::sym::FuncId>;

} // namespace frontend::internal
