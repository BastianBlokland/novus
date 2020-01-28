#pragma once
#include "backend/builder.hpp"
#include "catch2/catch.hpp"
#include "vm/executor.hpp"
#include "vm/platform/memory_interface.hpp"
#include <functional>

namespace vm {

inline auto buildAssemblyExpr(const std::function<void(backend::Builder*)>& build) -> vm::Assembly {
  auto builder = backend::Builder{};
  builder.label("entrypoint");
  build(&builder);
  builder.addRet();
  builder.addEntryPoint("entrypoint");
  return builder.close();
}

inline auto buildAssembly(const std::function<void(backend::Builder*)>& build) -> vm::Assembly {
  auto builder = backend::Builder{};
  build(&builder);
  return builder.close();
}

#define CHECK_ASM(ASM, INPUT, ENV_VARS, ENV_ARG, ...)                                              \
  {                                                                                                \
    auto memInterface = platform::MemoryInterface{};                                               \
    memInterface.setStdIn(INPUT);                                                                  \
    memInterface.setEnvVars(ENV_VARS);                                                             \
    memInterface.setEnvArgs(ENV_ARG);                                                              \
    execute(ASM, memInterface);                                                                    \
    const std::vector<std::string> expectedOutput = {__VA_ARGS__};                                 \
    CHECK_THAT(memInterface.getStdOut(), Catch::Equals(expectedOutput));                           \
  }

#define CHECK_ASM_THROWS(ASM, INPUT, EXCEPTION_TYPE)                                               \
  {                                                                                                \
    auto memInterface = platform::MemoryInterface{};                                               \
    memInterface.setStdIn(INPUT);                                                                  \
    CHECK_THROWS_AS(execute(ASM, memInterface), EXCEPTION_TYPE);                                   \
  }

#define CHECK_EXPR(BUILD, INPUT, ...)                                                              \
  CHECK_ASM(buildAssemblyExpr(BUILD), INPUT, {}, {}, __VA_ARGS__)

#define CHECK_EXPR_THROWS(BUILD, INPUT, ...)                                                       \
  CHECK_ASM_THROWS(buildAssemblyExpr(BUILD), INPUT, __VA_ARGS__)

#define CHECK_PROG(BUILD, INPUT, ...) CHECK_ASM(buildAssembly(BUILD), INPUT, {}, {}, __VA_ARGS__)

#define CHECK_PROG_WITH_ENV_VARS(BUILD, INPUT, ENV_VAR_SET, ...)                                   \
  CHECK_ASM(buildAssembly(BUILD), INPUT, ENV_VAR_SET, {}, __VA_ARGS__)

#define CHECK_PROG_WITH_ENV_ARGS(BUILD, INPUT, ENV_ARG_SET, ...)                                   \
  CHECK_ASM(buildAssembly(BUILD), INPUT, {}, ENV_ARG_SET, __VA_ARGS__)

#define CHECK_PROG_THROWS(BUILD, INPUT, ...)                                                       \
  CHECK_ASM_THROWS(buildAssembly(BUILD), INPUT, __VA_ARGS__)

} // namespace vm
