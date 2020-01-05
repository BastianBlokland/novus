#pragma once
#include "backend/builder.hpp"
#include "prog/program.hpp"

namespace backend::internal {

auto generateStructEquality(
    Builder* builder, const prog::Program& program, const prog::sym::StructDef& structDef) -> void;

auto generateUnionEquality(
    Builder* builder, const prog::Program& program, const prog::sym::UnionDef& unionDef) -> void;

} // namespace backend::internal
