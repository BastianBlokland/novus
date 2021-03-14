#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_assign.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_group.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing constant expressions", "frontend") {

  SECTION("Access function input") {
    const auto& output = ANALYZE("fun f(int a) a");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));
    const auto& consts  = funcDef.getConsts();
    CHECK(funcDef.getBody() == *prog::expr::constExprNode(consts, consts.lookup("a").value()));
  }

  SECTION("Declare and access local const") {
    const auto& output = ANALYZE("fun f() a = 1; b = a; b");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");
    const auto& consts  = funcDef.getConsts();

    const auto a = consts.lookup("a");
    REQUIRE(a);
    const auto b = consts.lookup("b");
    REQUIRE(b);

    auto groupExpr = prog::expr::groupExprNode(EXPRS(
        prog::expr::assignExprNode(consts, a.value(), prog::expr::litIntNode(output.getProg(), 1)),
        prog::expr::assignExprNode(consts, b.value(), prog::expr::constExprNode(consts, a.value())),
        prog::expr::constExprNode(consts, b.value())));

    CHECK(funcDef.getBody() == *groupExpr);
  }

  SECTION("Access const in anonymous function") {
    const auto& output = ANALYZE("fun f() lambda () x = 1; x");
    REQUIRE(output.isSuccess());
    const auto& anonDef = findAnonFuncDef(output, 0);
    const auto& consts  = anonDef.getConsts();

    const auto x = consts.lookup("x");
    REQUIRE(x);

    auto groupExpr = prog::expr::groupExprNode(EXPRS(
        prog::expr::assignExprNode(consts, x.value(), prog::expr::litIntNode(output.getProg(), 1)),
        prog::expr::constExprNode(consts, x.value())));

    CHECK(anonDef.getBody() == *groupExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("fun f() -> int x", errUndeclaredConst(NO_SRC, "x"));
    CHECK_DIAG("fun f() -> int bool = 42", errConstNameConflictsWithType(NO_SRC, "bool"));
    CHECK_DIAG("fun f(int a) -> int a = 42", errConstNameConflictsWithConst(NO_SRC, "a"));
    CHECK_DIAG(
        "fun f(int a) -> int "
        "if intrinsic{int_le_int}(a, 5) -> b = 1 "
        "else                           -> b = 2",
        errConstNameConflictsWithConst(NO_SRC, "b"));
    CHECK_DIAG(
        "fun f(int a) -> int "
        "if intrinsic{int_gt_int}(a, 5) -> b = 1 "
        "else                           -> b + 1",
        errUninitializedConst(NO_SRC, "b"));
    CHECK_DIAG(
        "fun f(int a) -> int "
        "if b = intrinsic{int_mul_int}(a, a); intrinsic{int_le_int}(b, 5) -> b "
        "else                                                             -> b + 1",
        errUninitializedConst(NO_SRC, "b"));
    CHECK_DIAG("fun f() -> int (true && (x = 5; false)); x", errUninitializedConst(NO_SRC, "x"));
  }
}

} // namespace frontend
