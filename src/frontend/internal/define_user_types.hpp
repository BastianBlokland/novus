#pragma once
#include "internal/context.hpp"

namespace frontend::internal {

auto defineType(
    Context* ctx,
    const TypeSubstitutionTable* typeSubTable,
    prog::sym::TypeId id,
    const parse::StructDeclStmtNode& n) -> bool;

inline auto defineType(Context* ctx, prog::sym::TypeId id, const parse::StructDeclStmtNode& n) {
  return defineType(ctx, nullptr, id, n);
}

auto defineType(
    Context* ctx,
    const TypeSubstitutionTable* typeSubTable,
    prog::sym::TypeId id,
    const parse::UnionDeclStmtNode& n) -> bool;

inline auto defineType(Context* ctx, prog::sym::TypeId id, const parse::UnionDeclStmtNode& n) {
  return defineType(ctx, nullptr, id, n);
}

auto defineType(Context* ctx, prog::sym::TypeId id, const parse::EnumDeclStmtNode& n) -> bool;

} // namespace frontend::internal
