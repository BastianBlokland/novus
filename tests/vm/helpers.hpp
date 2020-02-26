#pragma once
#include "catch2/catch.hpp"
#include "novasm/assembler.hpp"
#include "vm/platform/memory_interface.hpp"
#include "vm/vm.hpp"
#include <functional>

namespace vm {

inline auto buildAssemblyExpr(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto asmb = novasm::Assembler{};
  asmb.label("entrypoint");
  build(&asmb);
  asmb.addRet();
  asmb.setEntrypoint("entrypoint");
  return asmb.close();
}

inline auto buildAssembly(const std::function<void(novasm::Assembler*)>& build)
    -> novasm::Assembly {
  auto asmb = novasm::Assembler{};
  build(&asmb);
  return asmb.close();
}

#define CHECK_ASM(ASM, INPUT, ENV_VARS, ENV_ARG, ...)                                              \
  {                                                                                                \
    auto assembly     = ASM;                                                                       \
    auto memInterface = platform::MemoryInterface{};                                               \
    memInterface.setStdIn(INPUT);                                                                  \
    memInterface.setEnvVars(ENV_VARS);                                                             \
    memInterface.setEnvArgs(ENV_ARG);                                                              \
    run(&assembly, &memInterface);                                                                 \
    const std::vector<std::string> expectedOutput = {__VA_ARGS__};                                 \
    CHECK_THAT(memInterface.getStdOut(), Catch::Equals(expectedOutput));                           \
  }

#define CHECK_ASM_RESULTCODE(ASM, INPUT, EXPECTED)                                                 \
  {                                                                                                \
    auto assembly     = ASM;                                                                       \
    auto memInterface = platform::MemoryInterface{};                                               \
    memInterface.setStdIn(INPUT);                                                                  \
    CHECK(run(&assembly, &memInterface) == (EXPECTED));                                            \
  }

#define CHECK_EXPR(BUILD, INPUT, ...)                                                              \
  CHECK_ASM(buildAssemblyExpr(BUILD), INPUT, {}, {}, __VA_ARGS__)

#define CHECK_EXPR_RESULTCODE(BUILD, INPUT, ...)                                                   \
  CHECK_ASM_RESULTCODE(buildAssemblyExpr(BUILD), INPUT, __VA_ARGS__)

#define CHECK_PROG(BUILD, INPUT, ...) CHECK_ASM(buildAssembly(BUILD), INPUT, {}, {}, __VA_ARGS__)

#define CHECK_PROG_WITH_ENV_VARS(BUILD, INPUT, ENV_VAR_SET, ...)                                   \
  CHECK_ASM(buildAssembly(BUILD), INPUT, ENV_VAR_SET, {}, __VA_ARGS__)

#define CHECK_PROG_WITH_ENV_ARGS(BUILD, INPUT, ENV_ARG_SET, ...)                                   \
  CHECK_ASM(buildAssembly(BUILD), INPUT, {}, ENV_ARG_SET, __VA_ARGS__)

#define CHECK_PROG_RESULTCODE(BUILD, INPUT, ...)                                                   \
  CHECK_ASM_RESULTCODE(buildAssembly(BUILD), INPUT, __VA_ARGS__)

} // namespace vm
