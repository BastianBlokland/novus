#pragma once
#include "novasm/executable.hpp"
#include "prog/program.hpp"
#include <unordered_map>
#include <utility>

namespace backend {

using InstructionLabels = std::unordered_map<uint32_t, std::vector<std::string>>;

enum class GenerateFlags : unsigned int {
  None          = 0,
  Deterministic = 1 << 0,
};

// Generate a novus executable for the given program.
// Note: The returned instruction-labels can optionally be used to add human readable labels to the
// output executable, usefull for debugging.
auto generate(const prog::Program& program, GenerateFlags flags = GenerateFlags::None)
    -> std::pair<novasm::Executable, InstructionLabels>;

inline auto operator|(GenerateFlags lhs, GenerateFlags rhs) noexcept {
  return static_cast<GenerateFlags>(
      static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}

inline auto operator&(GenerateFlags lhs, GenerateFlags rhs) noexcept {
  return static_cast<GenerateFlags>(
      static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

} // namespace backend
