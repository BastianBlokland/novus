#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/operator.hpp"

namespace frontend {

TEST_CASE("Analyzing unary expressions", "[frontend]") {

  SECTION("Get basic unary expression") {
    const auto& output = ANALYZE("fun f(int a) -> int -a");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(
        prog::expr::constExprNode(funcDef.getConsts(), funcDef.getConsts().lookup("a").value()));

    CHECK(
        funcDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_OP_ID(output, prog::Operator::Minus, GET_TYPE_ID(output, "int")),
            std::move(args)));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f() -> int -false", errUndeclaredUnaryOperator(src, "-", "bool", input::Span{15, 15}));
  }
}

} // namespace frontend
