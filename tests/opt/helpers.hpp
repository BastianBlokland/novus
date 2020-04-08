#pragma once
#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "prog/operator.hpp"
#include "prog/program.hpp"

namespace opt {

#define SRC(INPUT)                                                                                 \
  frontend::buildSource("test", std::nullopt, std::string{INPUT}.begin(), std::string{INPUT}.end())

#define ANALYZE(INPUT) analyze(SRC(INPUT))

#define GET_TYPE_ID(PROG, TYPE_NAME) (PROG).lookupType(TYPE_NAME).value()

#define GET_OP_FUNC_ID(PROG, OPERATOR, ...)                                                        \
  PROG.lookupFunc(                                                                                 \
          prog::getFuncName(OPERATOR),                                                             \
          prog::sym::TypeSet{__VA_ARGS__},                                                         \
          prog::sym::OverloadOptions{0})                                                           \
      .value()

#define GET_FUNC_ID(PROG, FUNCNAME, ...)                                                           \
  PROG.lookupFunc(FUNCNAME, prog::sym::TypeSet{__VA_ARGS__}, prog::sym::OverloadOptions{0}).value()

#define GET_FUNC_DECL(PROG, FUNCNAME, ...)                                                         \
  PROG.getFuncDecl(GET_FUNC_ID(PROG, FUNCNAME, __VA_ARGS__))

#define GET_FUNC_DEF(PROG, FUNCNAME, ...)                                                          \
  (PROG).getFuncDef(GET_FUNC_ID(PROG, FUNCNAME, __VA_ARGS__))

#define ASSERT_EXPR(OPT, INPUT, EXPECTED_EXPR)                                                     \
  {                                                                                                \
    const auto& output = ANALYZE("act test() " INPUT);                                             \
    REQUIRE(output.isSuccess());                                                                   \
    const auto prog = OPT(output.getProg());                                                       \
    CHECK(GET_FUNC_DEF(prog, "test").getExpr() == *(EXPECTED_EXPR));                               \
  }

} // namespace opt
