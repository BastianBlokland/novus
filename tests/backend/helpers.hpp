#pragma once
#include "assembly_output.hpp"
#include "backend/generator.hpp"
#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "novasm/assembler.hpp"

namespace backend {

inline auto buildAssemblyExprInt(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto asmb = novasm::Assembler{};
  asmb.label("prog");
  build(&asmb);
  asmb.addConvIntString();
  asmb.addPCall(novasm::PCallCode::ConWriteString);
  asmb.addRet();
  asmb.setEntrypoint("prog");
  return asmb.close();
}

inline auto buildAssemblyExprLong(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto asmb = novasm::Assembler{};
  asmb.label("prog");
  build(&asmb);
  asmb.addConvLongString();
  asmb.addPCall(novasm::PCallCode::ConWriteString);
  asmb.addRet();
  asmb.setEntrypoint("prog");
  return asmb.close();
}

inline auto buildAssemblyExprFloat(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto asmb = novasm::Assembler{};
  asmb.label("prog");
  build(&asmb);
  asmb.addConvFloatString();
  asmb.addPCall(novasm::PCallCode::ConWriteString);
  asmb.addRet();
  asmb.setEntrypoint("prog");
  return asmb.close();
}

inline auto buildAssemblyExprBool(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto asmb = novasm::Assembler{};
  asmb.label("prog");
  build(&asmb);
  asmb.addConvBoolString();
  asmb.addPCall(novasm::PCallCode::ConWriteString);
  asmb.addRet();
  asmb.setEntrypoint("prog");
  return asmb.close();
}

inline auto buildAssemblyExprString(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto asmb = novasm::Assembler{};
  asmb.label("prog");
  build(&asmb);
  asmb.addPCall(novasm::PCallCode::ConWriteString);
  asmb.addRet();
  asmb.setEntrypoint("prog");
  return asmb.close();
}

inline auto buildAssemblyExprChar(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto asmb = novasm::Assembler{};
  asmb.label("prog");
  build(&asmb);
  asmb.addConvCharString();
  asmb.addPCall(novasm::PCallCode::ConWriteString);
  asmb.addRet();
  asmb.setEntrypoint("prog");
  return asmb.close();
}

inline auto buildAssembly(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto asmb = novasm::Assembler{};
  build(&asmb);
  return asmb.close();
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

#define CHECK_EXPR_LONG(INPUT, BUILD_EXPECTED_ASM)                                                 \
  CHECK_ASM(                                                                                       \
      "conWrite(string(" + std::string(INPUT) + "))", buildAssemblyExprLong(BUILD_EXPECTED_ASM))

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
