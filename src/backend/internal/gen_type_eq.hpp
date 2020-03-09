#pragma once
#include "novasm/assembler.hpp"
#include "prog/program.hpp"

namespace backend::internal {

auto generateUserTypeEquality(novasm::Assembler* asmb, const prog::Program& prog) -> void;

} // namespace backend::internal
