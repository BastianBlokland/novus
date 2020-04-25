#pragma once
#include "novasm/assembler.hpp"
#include "prog/program.hpp"

namespace backend::internal {

auto genUserTypeEquality(novasm::Assembler* asmb, const prog::Program& prog) -> void;

} // namespace backend::internal
