#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_switch.hpp"
#include "prog/expr/node_union_check.hpp"
#include "prog/expr/node_union_get.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing 'is' / 'as' expressions", "frontend") {

  SECTION("Get 'as' expression") {
    const auto& output = ANALYZE("union Val = int, bool "
                                 "fun f(Val v) "
                                 "  if v as int i -> i"
                                 "  else          -> 42");
    REQUIRE(output.isSuccess());

    const auto& funcDef    = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "Val"));
    const auto& funcConsts = funcDef.getConsts();

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(prog::expr::unionGetExprNode(
            output.getProg(),
            prog::expr::constExprNode(funcConsts, *funcConsts.lookup("v")),
            funcConsts,
            *funcConsts.lookup("i"))),
        EXPRS(
            prog::expr::constExprNode(funcConsts, *funcConsts.lookup("i")),
            prog::expr::litIntNode(output.getProg(), 42)));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Check 'is' expression") {
    const auto& output = ANALYZE("union Val = int, bool "
                                 "fun f(Val v) v is int");
    REQUIRE(output.isSuccess());

    const auto& funcDef    = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "Val"));
    const auto& funcConsts = funcDef.getConsts();

    CHECK(
        funcDef.getBody() ==
        *prog::expr::unionCheckExprNode(
            output.getProg(),
            prog::expr::constExprNode(funcConsts, *funcConsts.lookup("v")),
            GET_TYPE_ID(output, "int")));
  }

  SECTION("Check 'as' with discard expression") {
    const auto& output = ANALYZE("union Val = int, bool "
                                 "fun f(Val v) v as int _");
    REQUIRE(output.isSuccess());

    const auto& funcDef    = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "Val"));
    const auto& funcConsts = funcDef.getConsts();

    CHECK(
        funcDef.getBody() ==
        *prog::expr::unionCheckExprNode(
            output.getProg(),
            prog::expr::constExprNode(funcConsts, *funcConsts.lookup("v")),
            GET_TYPE_ID(output, "int")));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(" fun f(int i) i as bool b", errNonUnionIsExpression(src, input::Span{14, 14}));
    CHECK_DIAG(
        "struct S = int i "
        "fun f(S s) s as int i",
        errNonUnionIsExpression(src, input::Span{28, 28}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) u as hello i",
        errUndeclaredType(src, "hello", 0, input::Span{37, 41}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) u as bool b",
        errTypeNotPartOfUnion(src, "bool", "U", input::Span{37, 40}));

    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) !(u as int i) ? i : 42",
        errUncheckedAsExpressionWithConst(src, input::Span{39, 41}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) (u as int i; z = 4) ? i : 42",
        errUncheckedAsExpressionWithConst(src, input::Span{38, 40}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) -> int (u as int i || true) ? i : 42",
        errUncheckedAsExpressionWithConst(src, input::Span{45, 47}));

    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) u as int int ? 1 : 2",
        errConstNameConflictsWithType(src, "int", input::Span{41, 43}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) u as int u ? 1 : 2",
        errConstNameConflictsWithConst(src, "u", input::Span{41, 41}));
  }
}

} // namespace frontend
