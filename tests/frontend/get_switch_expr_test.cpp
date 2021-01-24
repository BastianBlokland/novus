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

    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(prog::expr::litBoolNode(output.getProg(), true));
    conditions.push_back(prog::expr::litBoolNode(output.getProg(), false));

    auto branches = std::vector<prog::expr::NodePtr>{};
    branches.push_back(prog::expr::litIntNode(output.getProg(), 1));
    branches.push_back(prog::expr::litIntNode(output.getProg(), 2));
    branches.push_back(prog::expr::litIntNode(output.getProg(), 3));
    auto switchExpr =
        prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Get switch expression with conversion on the branches") {
    const auto& output = ANALYZE("fun f() "
                                 "  if true   -> 1 "
                                 "  if false  -> 1.0 "
                                 "  else      -> 3");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");

    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(prog::expr::litBoolNode(output.getProg(), true));
    conditions.push_back(prog::expr::litBoolNode(output.getProg(), false));

    auto branches = std::vector<prog::expr::NodePtr>{};
    branches.push_back(
        applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 1)));
    branches.push_back(prog::expr::litFloatNode(output.getProg(), 1.0F));
    branches.push_back(
        applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 3)));
    auto switchExpr =
        prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Declare consts in switch expression conditions") {
    const auto& output = ANALYZE("fun f() -> int "
                                 "  if x = 1; true  -> x "
                                 "  else            -> 2");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");
    const auto& consts  = funcDef.getConsts();

    auto condSubExprs = std::vector<prog::expr::NodePtr>{};
    condSubExprs.push_back(prog::expr::assignExprNode(
        consts, consts.lookup("x").value(), prog::expr::litIntNode(output.getProg(), 1)));
    condSubExprs.push_back(prog::expr::litBoolNode(output.getProg(), true));

    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(prog::expr::groupExprNode(std::move(condSubExprs)));

    auto branches = std::vector<prog::expr::NodePtr>{};
    branches.push_back(prog::expr::constExprNode(consts, consts.lookup("x").value()));
    branches.push_back(prog::expr::litIntNode(output.getProg(), 2));
    auto switchExpr =
        prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches));

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

    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(prog::expr::unionGetExprNode(
        output.getProg(),
        prog::expr::constExprNode(consts, *consts.lookup("u")),
        consts,
        *consts.lookup("i")));
    conditions.push_back(prog::expr::unionCheckExprNode(
        output.getProg(),
        prog::expr::constExprNode(consts, *consts.lookup("u")),
        GET_TYPE_ID(output, "float")));

    auto branches = std::vector<prog::expr::NodePtr>{};
    branches.push_back(prog::expr::constExprNode(consts, *consts.lookup("i")));
    branches.push_back(prog::expr::litIntNode(output.getProg(), 0));
    branches.push_back(prog::expr::failNode(GET_TYPE_ID(output, "int")));
    auto switchExpr =
        prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches));

    CHECK(funcDef.getBody() == *switchExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f(bool a) "
        "  if a   -> 1 "
        "  else   -> true",
        errBranchesHaveNoCommonType(src, input::Span{16, 43}));
    CHECK_DIAG(
        "fun f(int a) -> int "
        "  if a   -> 1 "
        "  else   -> 2",
        errNoImplicitConversionFound(src, "int", "bool", input::Span{25, 25}));
    CHECK_DIAG(
        "fun f() -> int "
        "  if true   -> 1 "
        "  else      -> true",
        errNoImplicitConversionFound(src, "bool", "int", input::Span{34, 50}));
    CHECK_DIAG(
        "fun f() -> int "
        "  if true   -> 1 "
        "  if false  -> false "
        "  else      -> 2",
        errNoImplicitConversionFound(src, "bool", "int", input::Span{34, 51}));
    CHECK_DIAG(
        "fun f() -> int "
        "  if false  -> 2 ",
        errNonExhaustiveSwitchWithoutElse(src, input::Span{17, 30}));
    CHECK_DIAG(
        "union U = int, float "
        "fun f(U u) -> int "
        "  if u is float -> 1 ",
        errNonExhaustiveSwitchWithoutElse(src, input::Span{41, 58}));
  }
}

} // namespace frontend
