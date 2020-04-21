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

TEST_CASE("Analyzing binary expressions", "[frontend]") {

  SECTION("Get basic binary expression") {
    const auto& output = ANALYZE("fun f(int a) -> int 1 * a");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    args.push_back(
        prog::expr::constExprNode(funcDef.getConsts(), funcDef.getConsts().lookup("a").value()));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_OP_ID(
            output, prog::Operator::Star, GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "int")),
        std::move(args));

    CHECK(funcDef.getExpr() == *callExpr);
  }

  SECTION("Get binary expression with conversion on lhs") {
    const auto& output = ANALYZE("fun f(float a) 2 * a");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "float"));

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 2)));
    args.push_back(
        prog::expr::constExprNode(funcDef.getConsts(), funcDef.getConsts().lookup("a").value()));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_OP_ID(
            output,
            prog::Operator::Star,
            GET_TYPE_ID(output, "float"),
            GET_TYPE_ID(output, "float")),
        std::move(args));

    CHECK(funcDef.getExpr() == *callExpr);
  }

  SECTION("Get binary expression with conversion on rhs") {
    const auto& output = ANALYZE("fun f(float a) a * 2");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "float"));

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(
        prog::expr::constExprNode(funcDef.getConsts(), funcDef.getConsts().lookup("a").value()));
    args.push_back(applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 2)));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_OP_ID(
            output,
            prog::Operator::Star,
            GET_TYPE_ID(output, "float"),
            GET_TYPE_ID(output, "float")),
        std::move(args));

    CHECK(funcDef.getExpr() == *callExpr);
  }

  SECTION("Get logic 'and' expression") {
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
    auto switchExpr =
        prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches));

    CHECK(funcDef.getExpr() == *switchExpr);
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

    auto andConditions = std::vector<prog::expr::NodePtr>{};
    andConditions.push_back(prog::expr::unionGetExprNode(
        output.getProg(),
        prog::expr::constExprNode(consts, *consts.lookup("a")),
        consts,
        *consts.lookup("aVal")));

    auto andBranches = std::vector<prog::expr::NodePtr>{};
    andBranches.push_back(prog::expr::unionGetExprNode(
        output.getProg(),
        prog::expr::constExprNode(consts, *consts.lookup("b")),
        consts,
        *consts.lookup("bVal")));
    andBranches.push_back(prog::expr::litBoolNode(output.getProg(), false));

    auto switchConditions = std::vector<prog::expr::NodePtr>{};
    switchConditions.push_back(prog::expr::switchExprNode(
        output.getProg(), std::move(andConditions), std::move(andBranches)));

    auto switchBranches = std::vector<prog::expr::NodePtr>{};
    switchBranches.push_back(prog::expr::constExprNode(consts, consts.lookup("bVal").value()));
    switchBranches.push_back(prog::expr::litIntNode(output.getProg(), 0));
    auto switchExpr = prog::expr::switchExprNode(
        output.getProg(), std::move(switchConditions), std::move(switchBranches));

    CHECK(funcDef.getExpr() == *switchExpr);
  }

  SECTION("Get logic 'or' expression") {
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
    auto switchExpr =
        prog::expr::switchExprNode(output.getProg(), std::move(conditions), std::move(branches));

    CHECK(funcDef.getExpr() == *switchExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f() -> int true + false",
        errUndeclaredBinOperator(src, "+", "bool", "bool", input::Span{20, 20}));
    CHECK_DIAG(
        "fun f(int a, bool b) -> bool a && b",
        errNoImplicitConversionFound(src, "int", "bool", input::Span{29, 29}));
    CHECK_DIAG(
        "fun f(bool a, int b) -> bool a || b",
        errNoImplicitConversionFound(src, "int", "bool", input::Span{34, 34}));
  }
}

} // namespace frontend
