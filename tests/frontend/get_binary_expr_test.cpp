#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_switch.hpp"
#include "prog/expr/node_union_get.hpp"
#include "prog/operator.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing binary expressions", "frontend") {

  SECTION("Get basic binary expression") {
    const auto& output = ANALYZE("fun f(int a) -> int 1 * a");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_OP_ID(
            output, prog::Operator::Star, GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "int")),
        EXPRS(
            prog::expr::litIntNode(output.getProg(), 1),
            prog::expr::constExprNode(
                funcDef.getConsts(), funcDef.getConsts().lookup("a").value())));

    CHECK(funcDef.getBody() == *callExpr);
  }

  SECTION("Get binary expression with conversion on lhs") {
    const auto& output = ANALYZE("fun f(float a) 2 * a");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "float"));

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_OP_ID(
            output,
            prog::Operator::Star,
            GET_TYPE_ID(output, "float"),
            GET_TYPE_ID(output, "float")),
        EXPRS(
            applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 2)),
            prog::expr::constExprNode(
                funcDef.getConsts(), funcDef.getConsts().lookup("a").value())));

    CHECK(funcDef.getBody() == *callExpr);
  }

  SECTION("Get binary expression with conversion on rhs") {
    const auto& output = ANALYZE("fun f(float a) a * 2");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "float"));

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_OP_ID(
            output,
            prog::Operator::Star,
            GET_TYPE_ID(output, "float"),
            GET_TYPE_ID(output, "float")),
        EXPRS(
            prog::expr::constExprNode(funcDef.getConsts(), funcDef.getConsts().lookup("a").value()),
            applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 2))));

    CHECK(funcDef.getBody() == *callExpr);
  }

  SECTION("Get logic 'and' expression") {
    const auto& output = ANALYZE("fun f(bool a, bool b) -> bool a && b");
    REQUIRE(output.isSuccess());
    const auto& funcDef =
        GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "bool"), GET_TYPE_ID(output, "bool"));
    const auto& consts = funcDef.getConsts();

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(prog::expr::constExprNode(consts, consts.lookup("a").value())),
        EXPRS(
            prog::expr::constExprNode(consts, consts.lookup("b").value()),
            prog::expr::litBoolNode(output.getProg(), false)));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Chain 'as' checks with binary 'and' in switch") {
    const auto& output = ANALYZE("struct Null "
                                 "union Option = int, Null "
                                 "fun f(Option a, Option b) "
                                 "  if a as int aVal && b as int bVal -> bVal "
                                 "  else                              -> 0");
    REQUIRE(output.isSuccess());
    const auto& funcDef =
        GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "Option"), GET_TYPE_ID(output, "Option"));
    const auto& consts = funcDef.getConsts();

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(prog::expr::switchExprNode(
            output.getProg(),
            EXPRS(prog::expr::unionGetExprNode(
                output.getProg(),
                prog::expr::constExprNode(consts, *consts.lookup("a")),
                consts,
                *consts.lookup("aVal"))),
            EXPRS(
                prog::expr::unionGetExprNode(
                    output.getProg(),
                    prog::expr::constExprNode(consts, *consts.lookup("b")),
                    consts,
                    *consts.lookup("bVal")),
                prog::expr::litBoolNode(output.getProg(), false)))),
        EXPRS(
            prog::expr::constExprNode(consts, consts.lookup("bVal").value()),
            prog::expr::litIntNode(output.getProg(), 0)));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Get logic 'or' expression") {
    const auto& output = ANALYZE("fun f(bool a, bool b) -> bool a || b");
    REQUIRE(output.isSuccess());
    const auto& funcDef =
        GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "bool"), GET_TYPE_ID(output, "bool"));
    const auto& consts = funcDef.getConsts();

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(prog::expr::constExprNode(consts, consts.lookup("a").value())),
        EXPRS(
            prog::expr::litBoolNode(output.getProg(), true),
            prog::expr::constExprNode(consts, consts.lookup("b").value())));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f() -> int true + false", errUndeclaredBinOperator(NO_SRC, "+", "bool", "bool"));
    CHECK_DIAG(
        "fun f(int a, bool b) -> bool a && b", errNoImplicitConversionFound(NO_SRC, "int", "bool"));
    CHECK_DIAG(
        "fun f(bool a, int b) -> bool a || b", errNoImplicitConversionFound(NO_SRC, "int", "bool"));
  }
}

} // namespace frontend
