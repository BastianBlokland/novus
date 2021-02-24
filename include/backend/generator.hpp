#pragma once
#include "novasm/executable.hpp"
#include "prog/program.hpp"
#include <unordered_map>
#include <utility>

namespace backend {

using InstructionLabels = std::unordered_map<uint32_t, std::vector<std::string>>;

// Generate a novus executable for the given program.
// Note: The returned instruction-labels can optionally be used to add human readable labels to the
// output executable, usefull for debugging.
auto generate(const prog::Program& program) -> std::pair<novasm::Executable, InstructionLabels>;

} // namespace backend
