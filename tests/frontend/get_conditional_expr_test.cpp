#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_assign.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_group.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_switch.hpp"

namespace frontend {

TEST_CASE("Analyzing conditional expressions", "[frontend]") {

  SECTION("Get basic conditional expression") {
    const auto& output = ANALYZE("fun f() -> int "
                                 "true ? 1 : 3");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");

    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(prog::expr::litBoolNode(output.getProg(), true));

    auto branches = std::vector<prog::expr::NodePtr>{};
    branches.push_back(prog::expr::litIntNode(output.getProg(), 1));
    branches.push_back(prog::expr::litIntNode(output.getProg(), 3));

    CHECK(
        funcDef.getExpr() ==
        *prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches)));
  }

  SECTION("Declare consts in condition expression conditions") {
    const auto& output = ANALYZE("fun f() -> int "
                                 "(x = 1; true) ? x : 2");
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

    CHECK(
        funcDef.getExpr() ==
        *prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches)));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f(int a) -> int "
        "a ? 1 : 2",
        errNonBoolConditionExpression(src, "int", input::Span{20, 20}));
    CHECK_DIAG(
        "fun f() -> int "
        "true ? 1 : true",
        errMismatchedBranchTypes(src, "int", "bool", input::Span{26, 29}));
  }
}

} // namespace frontend
