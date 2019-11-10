#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_switch.hpp"
#include "prog/operator.hpp"

namespace frontend {

TEST_CASE("Get binary expression", "[frontend]") {

  SECTION("Get basic binary expression") {
    const auto& output = ANALYZE("fun f(int a) -> int 1 * a");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    args.push_back(
        prog::expr::constExprNode(funcDef.getConsts(), funcDef.getConsts().lookup("a").value()));

    CHECK(
        funcDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_OP_ID(
                output,
                prog::Operator::Star,
                GET_TYPE_ID(output, "int"),
                GET_TYPE_ID(output, "int")),
            std::move(args)));
  }

  SECTION("Get logic and expression") {
    const auto& output = ANALYZE("fun f(bool a, bool b) -> bool a && b");
    REQUIRE(output.isSuccess());
    const auto& funcDef =
        GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "bool"), GET_TYPE_ID(output, "bool"));
    const auto& consts = funcDef.getConsts();

    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(prog::expr::constExprNode(consts, consts.lookup("a").value()));

    auto branches = std::vector<prog::expr::NodePtr>{};
    branches.push_back(prog::expr::constExprNode(consts, consts.lookup("b").value()));
    branches.push_back(prog::expr::litBoolNode(output.getProg(), false));

    CHECK(
        funcDef.getExpr() ==
        *prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches)));
  }

  SECTION("Get logic or expression") {
    const auto& output = ANALYZE("fun f(bool a, bool b) -> bool a || b");
    REQUIRE(output.isSuccess());
    const auto& funcDef =
        GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "bool"), GET_TYPE_ID(output, "bool"));
    const auto& consts = funcDef.getConsts();

    auto conditions = std::vector<prog::expr::NodePtr>{};
    conditions.push_back(prog::expr::constExprNode(consts, consts.lookup("a").value()));

    auto branches = std::vector<prog::expr::NodePtr>{};
    branches.push_back(prog::expr::litBoolNode(output.getProg(), true));
    branches.push_back(prog::expr::constExprNode(consts, consts.lookup("b").value()));

    CHECK(
        funcDef.getExpr() ==
        *prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches)));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f() -> int true + false",
        errUndeclaredBinOperator(src, "+", "bool", "bool", input::Span{20, 20}));
    CHECK_DIAG(
        "fun f(int a, bool b) -> bool a && b",
        errNonBoolExpressionInLogicOp(src, "int", input::Span{29, 29}));
    CHECK_DIAG(
        "fun f(bool a, int b) -> bool a || b",
        errNonBoolExpressionInLogicOp(src, "int", input::Span{34, 34}));
  }
}

} // namespace frontend
