#pragma once
#include "backend/generator.hpp"
#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"

namespace novasm {

inline auto generateAssembly(std::string input) {
  const auto src = frontend::buildSource("test", std::nullopt, input.begin(), input.end());
  const auto frontendOutput = frontend::analyze(src);
  REQUIRE(frontendOutput.isSuccess());
  return backend::generate(frontendOutput.getProg(), backend::GenerateFlags::Deterministic).first;
}

#define GEN_ASM(INPUT) generateAssembly(INPUT)

} // namespace novasm
