#pragma once
#include "novasm/assembly.hpp"
#include "prog/program.hpp"
#include <unordered_map>
#include <utility>

namespace backend {

using InstructionLabels = std::unordered_map<uint32_t, std::vector<std::string>>;

auto generate(const prog::Program& program) -> std::pair<novasm::Assembly, InstructionLabels>;

} // namespace backend
