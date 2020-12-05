#pragma once
#include "assembly_output.hpp"
#include "backend/generator.hpp"
#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "novasm/assembler.hpp"

namespace backend {

inline auto buildAssemblyExpr(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto compilerVersion = std::string{"0.42.1337"};
  auto asmb            = novasm::Assembler{std::move(compilerVersion)};
  build(&asmb);
  asmb.addRet();

  // Empty entry-point.
  auto entryPointLabel = asmb.generateLabel("entrypoint");
  asmb.setEntrypoint(entryPointLabel);
  asmb.label(entryPointLabel);
  asmb.addRet(); // Returning from the root stack-frame causes the program to stop.

  return asmb.close();
}

inline auto buildAssembly(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto compilerVersion = std::string{"0.42.1337"};
  auto asmb            = novasm::Assembler{std::move(compilerVersion)};
  build(&asmb);
  return asmb.close();
}

#define CHECK_ASM(INPUT, EXPECTED_ASM)                                                             \
  {                                                                                                \
    const auto srcText = std::string{INPUT};                                                       \
    const auto src = frontend::buildSource("test", std::nullopt, srcText.begin(), srcText.end());  \
    const auto frontendOutput = frontend::analyze(src);                                            \
    REQUIRE(frontendOutput.isSuccess());                                                           \
    const auto asmOutput = backend::generate(frontendOutput.getProg());                            \
    CHECK(asmOutput.first == (EXPECTED_ASM));                                                      \
  }

#define CHECK_EXPR(INPUT, BUILD_EXPECTED_ASM)                                                      \
  CHECK_ASM("act test() " + std::string(INPUT), buildAssemblyExpr(BUILD_EXPECTED_ASM))

#define CHECK_PROG(INPUT, BUILD_EXPECTED_ASM) CHECK_ASM(INPUT, buildAssembly(BUILD_EXPECTED_ASM))

} // namespace backend
