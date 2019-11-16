#pragma once
#include "backend/dasm/instruction.hpp"
#include "vm/assembly.hpp"
#include <vector>

namespace backend::dasm {

auto disassembleInstructions(const vm::Assembly& assembly) -> std::vector<Instruction>;

} // namespace backend::dasm
