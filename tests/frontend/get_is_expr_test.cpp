#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_switch.hpp"
#include "prog/expr/node_union_check.hpp"
#include "prog/expr/node_union_get.hpp"

namespace frontend {

TEST_CASE("Analyzing 'is' expressions", "[frontend]") {

  SECTION("Get 'is' expression") {
    const auto& output = ANALYZE("union Val = int, bool "
                                 "fun f(Val v) "
                                 "  if v is int i -> i"
                                 "  else          -> 42");
    REQUIRE(output.isSuccess());

    const auto& funcDef    = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "Val"));
    const auto& funcConsts = funcDef.getConsts();

    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(prog::expr::unionGetExprNode(
        output.getProg(),
        prog::expr::constExprNode(funcConsts, *funcConsts.lookup("v")),
        funcConsts,
        *funcConsts.lookup("i")));

    auto branches = std::vector<prog::expr::NodePtr>{};
    branches.push_back(prog::expr::constExprNode(funcConsts, *funcConsts.lookup("i")));
    branches.push_back(prog::expr::litIntNode(output.getProg(), 42)); // NOLINT: Magic numbers

    CHECK(
        funcDef.getExpr() ==
        *prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches)));
  }

  SECTION("Check 'is' expression") {
    const auto& output = ANALYZE("union Val = int, bool "
                                 "fun f(Val v) v is int _");
    REQUIRE(output.isSuccess());

    const auto& funcDef    = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "Val"));
    const auto& funcConsts = funcDef.getConsts();

    CHECK(
        funcDef.getExpr() ==
        *prog::expr::unionCheckExprNode(
            output.getProg(),
            prog::expr::constExprNode(funcConsts, *funcConsts.lookup("v")),
            GET_TYPE_ID(output, "int")));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(" fun f(int i) i is bool b", errNonUnionIsExpression(src, input::Span{14, 14}));
    CHECK_DIAG(
        "struct S = int i "
        "fun f(S s) s is int i",
        errNonUnionIsExpression(src, input::Span{28, 28}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) u is hello i",
        errUndeclaredType(src, "hello", input::Span{37, 41}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) u is bool b",
        errTypeNotPartOfUnion(src, "bool", "U", input::Span{37, 40}));

    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) !(u is int i) ? i : 42",
        errUncheckedIsExpressionWithConst(src, input::Span{39, 41}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) (u is int i; z = 4) ? i : 42",
        errUncheckedIsExpressionWithConst(src, input::Span{38, 40}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) (u is int i || true) ? i : 42",
        errUncheckedIsExpressionWithConst(src, input::Span{38, 40}));

    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) u is int int ? 1 : 2",
        errConstNameConflictsWithType(src, "int", input::Span{41, 43}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) u is int f ? 1 : 2",
        errConstNameConflictsWithFunction(src, "f", input::Span{41, 41}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) u is int print ? 1 : 2",
        errConstNameConflictsWithAction(src, "print", input::Span{41, 45}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) u is int u ? 1 : 2",
        errConstNameConflictsWithConst(src, "u", input::Span{41, 41}));
  }
}

} // namespace frontend
