#pragma once
#include "novasm/assembler.hpp"
#include "prog/program.hpp"

namespace backend::internal {

auto generateStructEquality(
    novasm::Assembler* asmb, const prog::Program& program, const prog::sym::StructDef& structDef)
    -> void;

auto generateUnionEquality(
    novasm::Assembler* asmb, const prog::Program& program, const prog::sym::UnionDef& unionDef)
    -> void;

} // namespace backend::internal
