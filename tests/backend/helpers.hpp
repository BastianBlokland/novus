#pragma once
#include "assembly_output.hpp"
#include "backend/builder.hpp"
#include "backend/generator.hpp"
#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"

namespace backend {

inline auto buildAssemblyExprInt(const std::function<void(backend::Builder*)>& build)
    -> vm::Assembly {
  auto builder = backend::Builder{};
  builder.label("print");
  build(&builder);
  builder.addPrintInt();
  builder.addRet();
  builder.addFail();
  builder.addEntryPoint("print");
  return builder.close();
}

inline auto buildAssemblyExprBool(const std::function<void(backend::Builder*)>& build)
    -> vm::Assembly {
  auto builder = backend::Builder{};
  builder.label("print");
  build(&builder);
  builder.addPrintLogic();
  builder.addRet();
  builder.addFail();
  builder.addEntryPoint("print");
  return builder.close();
}

inline auto buildAssembly(const std::function<void(backend::Builder*)>& build) -> vm::Assembly {
  auto builder = backend::Builder{};
  build(&builder);
  return builder.close();
}

#define CHECK_ASM(INPUT, EXPECTED_ASM)                                                             \
  {                                                                                                \
    const auto srcText        = std::string{INPUT};                                                \
    const auto src            = frontend::buildSource("test", srcText.begin(), srcText.end());     \
    const auto frontendOutput = frontend::analyze(src);                                            \
    REQUIRE(frontendOutput.isSuccess());                                                           \
    const auto assembly = backend::generate(frontendOutput.getProg());                             \
    CHECK(assembly == (EXPECTED_ASM));                                                             \
  }

#define CHECK_EXPR_INT(INPUT, BUILD_EXPECTED_ASM)                                                  \
  CHECK_ASM("print(" + std::string(INPUT) + ")", buildAssemblyExprInt(BUILD_EXPECTED_ASM))

#define CHECK_EXPR_BOOL(INPUT, BUILD_EXPECTED_ASM)                                                 \
  CHECK_ASM("print(" + std::string(INPUT) + ")", buildAssemblyExprBool(BUILD_EXPECTED_ASM))

#define CHECK_PROG(INPUT, BUILD_EXPECTED_ASM) CHECK_ASM(INPUT, buildAssembly(BUILD_EXPECTED_ASM))

} // namespace backend
