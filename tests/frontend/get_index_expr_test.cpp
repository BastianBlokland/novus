#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing index expressions", "frontend") {

  SECTION("Get single argument index operator") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "struct Pair = int a, int b "
                                 "fun [](Pair p, int i) "
                                 "  if i == 0 -> p.a "
                                 "  else      -> p.b "
                                 "fun f(Pair p) p[0]");
    REQUIRE(output.isSuccess());
    const auto& func = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "Pair"));

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(
            output, "__op_squaresquare", GET_TYPE_ID(output, "Pair"), GET_TYPE_ID(output, "int")),
        EXPRS(
            prog::expr::constExprNode(func.getConsts(), *func.getConsts().lookup("p")),
            prog::expr::litIntNode(output.getProg(), 0)));

    CHECK(func.getBody() == *callExpr);
  }

  SECTION("Get multi argument index operator") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "struct Pair = string a, string b "
                                 "fun [](Pair p, int i, string b) "
                                 "  if i == 0 -> Pair(p.a, b) "
                                 "  else      -> Pair(p.b, b) "
                                 "fun f(Pair p) p[0, \"hello world\"]");
    REQUIRE(output.isSuccess());
    const auto& func = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "Pair"));

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(
            output,
            "__op_squaresquare",
            GET_TYPE_ID(output, "Pair"),
            GET_TYPE_ID(output, "int"),
            GET_TYPE_ID(output, "string")),
        EXPRS(
            prog::expr::constExprNode(func.getConsts(), *func.getConsts().lookup("p")),
            prog::expr::litIntNode(output.getProg(), 0),
            prog::expr::litStringNode(output.getProg(), "hello world")));

    CHECK(func.getBody() == *callExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("fun f(int i) -> int i[.0]", errUndeclaredIndexOperator(NO_SRC, {"int", "float"}));
    CHECK_DIAG(
        "struct test = int i "
        "fun f(test t) -> int t[0]",
        errUndeclaredIndexOperator(NO_SRC, {"test", "int"}));
    CHECK_DIAG("fun f() -> int 42[0]", errUndeclaredIndexOperator(NO_SRC, {"int", "int"}));
  }
}

} // namespace frontend
