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

#define CHECK_ASM(ASM, ...)                                                                        \
  {                                                                                                \
    auto memInterface = platform::MemoryInterface{};                                               \
    execute(ASM, memInterface);                                                                    \
    const std::vector<std::string> expectedOutput = {__VA_ARGS__};                                 \
    CHECK_THAT(memInterface.getStdOut(), Catch::Equals(expectedOutput));                           \
  }

#define CHECK_ASM_THROWS(ASM, EXCEPTION_TYPE)                                                      \
  {                                                                                                \
    auto memInterface = platform::MemoryInterface{};                                               \
    CHECK_THROWS_AS(execute(ASM, memInterface), EXCEPTION_TYPE);                                   \
  }

#define CHECK_EXPR(BUILD, ...) CHECK_ASM(buildAssemblyExpr(BUILD), __VA_ARGS__)

#define CHECK_EXPR_THROWS(BUILD, ...) CHECK_ASM_THROWS(buildAssemblyExpr(BUILD), __VA_ARGS__)

#define CHECK_PROG(BUILD, ...) CHECK_ASM(buildAssembly(BUILD), __VA_ARGS__)

#define CHECK_PROG_THROWS(BUILD, ...) CHECK_ASM_THROWS(buildAssembly(BUILD), __VA_ARGS__)

} // namespace vm
