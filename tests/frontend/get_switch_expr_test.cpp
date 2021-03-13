#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_assign.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_fail.hpp"
#include "prog/expr/node_group.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_float.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_switch.hpp"
#include "prog/expr/node_union_check.hpp"
#include "prog/expr/node_union_get.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing switch expressions", "frontend") {

  SECTION("Get basic switch expression") {
    const auto& output = ANALYZE("fun f() -> int "
                                 "  if true   -> 1 "
                                 "  if false  -> 2 "
                                 "  else      -> 3");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(
            prog::expr::litBoolNode(output.getProg(), true),
            prog::expr::litBoolNode(output.getProg(), false)),
        EXPRS(
            prog::expr::litIntNode(output.getProg(), 1),
            prog::expr::litIntNode(output.getProg(), 2),
            prog::expr::litIntNode(output.getProg(), 3)));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Get switch expression with conversion on the branches") {
    const auto& output = ANALYZE("fun implicit float(int i) intrinsic{int_to_float}(i) "
                                 "fun f() "
                                 "  if true   -> 1 "
                                 "  if false  -> 1.0 "
                                 "  else      -> 3");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(
            prog::expr::litBoolNode(output.getProg(), true),
            prog::expr::litBoolNode(output.getProg(), false)),
        EXPRS(
            applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 1)),
            prog::expr::litFloatNode(output.getProg(), 1.0F),
            applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 3))));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Declare consts in switch expression conditions") {
    const auto& output = ANALYZE("fun f() -> int "
                                 "  if x = 1; true  -> x "
                                 "  else            -> 2");
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

  SECTION("Exhaustive union check switch expression") {
    const auto& output = ANALYZE("union U = int, float "
                                 "fun f(U u) -> int "
                                 "  if u as int i -> i "
                                 "  if u is float -> 0");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "U"));
    const auto& consts  = funcDef.getConsts();

    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(),
        EXPRS(
            prog::expr::unionGetExprNode(
                output.getProg(),
                prog::expr::constExprNode(consts, *consts.lookup("u")),
                consts,
                *consts.lookup("i")),
            prog::expr::unionCheckExprNode(
                output.getProg(),
                prog::expr::constExprNode(consts, *consts.lookup("u")),
                GET_TYPE_ID(output, "float"))),
        EXPRS(
            prog::expr::constExprNode(consts, *consts.lookup("i")),
            prog::expr::litIntNode(output.getProg(), 0),
            prog::expr::failNode(GET_TYPE_ID(output, "int"))));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f(bool a) "
        "  if a   -> 1 "
        "  else   -> true",
        errBranchesHaveNoCommonType(NO_SRC));
    CHECK_DIAG(
        "fun f(int a) -> int "
        "  if a   -> 1 "
        "  else   -> 2",
        errNoImplicitConversionFound(NO_SRC, "int", "bool"));
    CHECK_DIAG(
        "fun f() -> int "
        "  if true   -> 1 "
        "  else      -> true",
        errNoImplicitConversionFound(NO_SRC, "bool", "int"));
    CHECK_DIAG(
        "fun f() -> int "
        "  if true   -> 1 "
        "  if false  -> false "
        "  else      -> 2",
        errNoImplicitConversionFound(NO_SRC, "bool", "int"));
    CHECK_DIAG(
        "fun f() -> int "
        "  if false  -> 2 ",
        errNonExhaustiveSwitchWithoutElse(NO_SRC));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) -> int "
        "  if u is float -> 1 ",
        errNonExhaustiveSwitchWithoutElse(NO_SRC));
  }
}

} // namespace frontend
