#pragma once
#include "assembly_output.hpp"
#include "backend/generator.hpp"
#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "novasm/assembler.hpp"

namespace backend {

inline auto buildExecutableExpr(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Executable {
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

inline auto buildExecutable(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Executable {
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
  CHECK_ASM("act test() " + std::string(INPUT), buildExecutableExpr(BUILD_EXPECTED_ASM))

#define CHECK_EXPR_BOOL(INPUT, BUILD_EXPECTED_ASM)                                                 \
  CHECK_ASM("act test() -> bool " + std::string(INPUT), buildExecutableExpr(BUILD_EXPECTED_ASM))

#define CHECK_EXPR_FLOAT(INPUT, BUILD_EXPECTED_ASM)                                                \
  CHECK_ASM("act test() -> float " + std::string(INPUT), buildExecutableExpr(BUILD_EXPECTED_ASM))

#define CHECK_EXPR_INT(INPUT, BUILD_EXPECTED_ASM)                                                  \
  CHECK_ASM("act test() -> int " + std::string(INPUT), buildExecutableExpr(BUILD_EXPECTED_ASM))

#define CHECK_EXPR_LONG(INPUT, BUILD_EXPECTED_ASM)                                                 \
  CHECK_ASM("act test() -> long " + std::string(INPUT), buildExecutableExpr(BUILD_EXPECTED_ASM))

#define CHECK_EXPR_CHAR(INPUT, BUILD_EXPECTED_ASM)                                                 \
  CHECK_ASM("act test() -> char " + std::string(INPUT), buildExecutableExpr(BUILD_EXPECTED_ASM))

#define CHECK_EXPR_STRING(INPUT, BUILD_EXPECTED_ASM)                                               \
  CHECK_ASM("act test() -> string " + std::string(INPUT), buildExecutableExpr(BUILD_EXPECTED_ASM))

#define CHECK_PROG(INPUT, BUILD_EXPECTED_ASM) CHECK_ASM(INPUT, buildExecutable(BUILD_EXPECTED_ASM))

} // namespace backend
