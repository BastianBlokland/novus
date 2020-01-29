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
  builder.label("prog");
  build(&builder);
  builder.addConvIntString();
  builder.addPCall(vm::PCallCode::ConWriteString);
  builder.addPop();
  builder.addRet();
  builder.addEntryPoint("prog");
  return builder.close();
}

inline auto buildAssemblyExprFloat(const std::function<void(backend::Builder*)>& build)
    -> vm::Assembly {
  auto builder = backend::Builder{};
  builder.label("prog");
  build(&builder);
  builder.addConvFloatString();
  builder.addPCall(vm::PCallCode::ConWriteString);
  builder.addPop();
  builder.addRet();
  builder.addEntryPoint("prog");
  return builder.close();
}

inline auto buildAssemblyExprBool(const std::function<void(backend::Builder*)>& build)
    -> vm::Assembly {
  auto builder = backend::Builder{};
  builder.label("prog");
  build(&builder);
  builder.addConvBoolString();
  builder.addPCall(vm::PCallCode::ConWriteString);
  builder.addPop();
  builder.addRet();
  builder.addEntryPoint("prog");
  return builder.close();
}

inline auto buildAssemblyExprString(const std::function<void(backend::Builder*)>& build)
    -> vm::Assembly {
  auto builder = backend::Builder{};
  builder.label("prog");
  build(&builder);
  builder.addPCall(vm::PCallCode::ConWriteString);
  builder.addPop();
  builder.addRet();
  builder.addEntryPoint("prog");
  return builder.close();
}

inline auto buildAssemblyExprChar(const std::function<void(backend::Builder*)>& build)
    -> vm::Assembly {
  auto builder = backend::Builder{};
  builder.label("prog");
  build(&builder);
  builder.addConvCharString();
  builder.addPCall(vm::PCallCode::ConWriteString);
  builder.addPop();
  builder.addRet();
  builder.addEntryPoint("prog");
  return builder.close();
}

inline auto buildAssembly(const std::function<void(backend::Builder*)>& build) -> vm::Assembly {
  auto builder = backend::Builder{};
  build(&builder);
  return builder.close();
}

#define CHECK_ASM(INPUT, EXPECTED_ASM)                                                             \
  {                                                                                                \
    const auto srcText = std::string{INPUT};                                                       \
    const auto src = frontend::buildSource("test", std::nullopt, srcText.begin(), srcText.end());  \
    const auto frontendOutput = frontend::analyze(src);                                            \
    REQUIRE(frontendOutput.isSuccess());                                                           \
    const auto assembly = backend::generate(frontendOutput.getProg());                             \
    CHECK(assembly == (EXPECTED_ASM));                                                             \
  }

#define CHECK_EXPR_INT(INPUT, BUILD_EXPECTED_ASM)                                                  \
  CHECK_ASM(                                                                                       \
      "conWrite(string(" + std::string(INPUT) + "))", buildAssemblyExprInt(BUILD_EXPECTED_ASM))

#define CHECK_EXPR_FLOAT(INPUT, BUILD_EXPECTED_ASM)                                                \
  CHECK_ASM(                                                                                       \
      "conWrite(string(" + std::string(INPUT) + "))", buildAssemblyExprFloat(BUILD_EXPECTED_ASM))

#define CHECK_EXPR_BOOL(INPUT, BUILD_EXPECTED_ASM)                                                 \
  CHECK_ASM(                                                                                       \
      "conWrite(string(" + std::string(INPUT) + "))", buildAssemblyExprBool(BUILD_EXPECTED_ASM))

#define CHECK_EXPR_STRING(INPUT, BUILD_EXPECTED_ASM)                                               \
  CHECK_ASM("conWrite(" + std::string(INPUT) + ")", buildAssemblyExprString(BUILD_EXPECTED_ASM))

#define CHECK_EXPR_CHAR(INPUT, BUILD_EXPECTED_ASM)                                                 \
  CHECK_ASM(                                                                                       \
      "conWrite(string(" + std::string(INPUT) + "))", buildAssemblyExprChar(BUILD_EXPECTED_ASM))

#define CHECK_PROG(INPUT, BUILD_EXPECTED_ASM) CHECK_ASM(INPUT, buildAssembly(BUILD_EXPECTED_ASM))

} // namespace backend
