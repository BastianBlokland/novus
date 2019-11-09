#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"
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

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f() -> int 1 && 1",
        errUndeclaredBinOperator(src, "&&", "int", "int", input::Span{17, 18}));
  }
}

} // namespace frontend
