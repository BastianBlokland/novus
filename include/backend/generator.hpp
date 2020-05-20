#pragma once
#include "novasm/assembly.hpp"
#include "prog/program.hpp"
#include <unordered_map>
#include <utility>

namespace backend {

using InstructionLabels = std::unordered_map<uint32_t, std::vector<std::string>>;

// Generate novus assembly for the given program.
// Note: The returned instruction-labels can optionally be used to add human readable labels to the
// output assembly, usefull for debugging.
auto generate(const prog::Program& program) -> std::pair<novasm::Assembly, InstructionLabels>;

} // namespace backend
