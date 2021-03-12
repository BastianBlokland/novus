#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_assign.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_group.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_float.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_switch.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing conditional expressions", "frontend") {

  SECTION("Get basic conditional expression") {
    const auto& output = ANALYZE("fun f() -> int "
                                 "true ? 1 : 3");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(prog::expr::litBoolNode(output.getProg(), true)),
        EXPRS(
            prog::expr::litIntNode(output.getProg(), 1),
            prog::expr::litIntNode(output.getProg(), 3)));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Get conditional expression with conversion on the lhs branch") {
    const auto& output = ANALYZE("fun implicit float(int i) intrinsic{int_to_float}(i) "
                                 "fun f() true ? 0 : 1.0");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(prog::expr::litBoolNode(output.getProg(), true)),
        EXPRS(
            applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 0)),
            prog::expr::litFloatNode(output.getProg(), 1.0F)));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Get conditional expression with conversion on the rhs branch") {
    const auto& output = ANALYZE("fun implicit float(int i) intrinsic{int_to_float}(i) "
                                 "fun f() true ? 1.0 : 0");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(prog::expr::litBoolNode(output.getProg(), true)),
        EXPRS(
            prog::expr::litFloatNode(output.getProg(), 1.0F),
            applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 0))));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Declare consts in condition expression conditions") {
    const auto& output = ANALYZE("fun f() -> int "
                                 "(x = 1; true) ? x : 2");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");
    const auto& consts  = funcDef.getConsts();

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(prog::expr::groupExprNode(EXPRS(
            prog::expr::assignExprNode(
                consts, consts.lookup("x").value(), prog::expr::litIntNode(output.getProg(), 1)),
            prog::expr::litBoolNode(output.getProg(), true)))),
        EXPRS(
            prog::expr::constExprNode(consts, consts.lookup("x").value()),
            prog::expr::litIntNode(output.getProg(), 2)));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f(int a) -> int "
        "a ? 1 : 2",
        errNoImplicitConversionFound(NO_SRC, "int", "bool"));
    CHECK_DIAG(
        "fun f() -> int "
        "true ? 1 : true",
        errNoImplicitConversionFound(NO_SRC, "bool", "int"));
  }
}

} // namespace frontend
