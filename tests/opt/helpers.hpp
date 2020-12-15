#pragma once
#include "catch2/catch.hpp"
#include "frontend/analysis.hpp"
#include "frontend/source.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_long.hpp"
#include "prog/operator.hpp"
#include "prog/program.hpp"

namespace opt {

inline auto buildSource(std::string input) {
  return frontend::buildSource("test", std::nullopt, input.begin(), input.end());
}

#define SRC(INPUT) buildSource(std::string{INPUT})

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

inline auto
getIntBinaryOpExpr(const prog::Program& prog, prog::Operator op, int32_t lhs, int32_t rhs)
    -> prog::expr::NodePtr {

  const auto funcId = GET_OP_FUNC_ID(prog, op, GET_TYPE_ID(prog, "int"), GET_TYPE_ID(prog, "int"));

  auto args = std::vector<prog::expr::NodePtr>{};
  args.push_back(prog::expr::litIntNode(prog, lhs));
  args.push_back(prog::expr::litIntNode(prog, rhs));
  return prog::expr::callExprNode(prog, funcId, std::move(args));
}

inline auto
getLongBinaryOpExpr(const prog::Program& prog, prog::Operator op, int64_t lhs, int64_t rhs)
    -> prog::expr::NodePtr {

  const auto funcId =
      GET_OP_FUNC_ID(prog, op, GET_TYPE_ID(prog, "long"), GET_TYPE_ID(prog, "long"));

  auto args = std::vector<prog::expr::NodePtr>{};
  args.push_back(prog::expr::litLongNode(prog, lhs));
  args.push_back(prog::expr::litLongNode(prog, rhs));
  return prog::expr::callExprNode(prog, funcId, std::move(args));
}

} // namespace opt
