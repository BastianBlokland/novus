#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/operator.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing parenthesized expressions", "frontend") {

  SECTION("Get basic paren expression") {
    const auto& output = ANALYZE("fun f(int a, int b) ((a) + (b))");
    REQUIRE(output.isSuccess());
    const auto& funcDef =
        GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "int"));

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(
        prog::expr::constExprNode(funcDef.getConsts(), funcDef.getConsts().lookup("a").value()));
    args.push_back(
        prog::expr::constExprNode(funcDef.getConsts(), funcDef.getConsts().lookup("b").value()));
    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_OP_ID(
            output, prog::Operator::Plus, GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "int")),
        std::move(args));

    CHECK(funcDef.getExpr() == *callExpr);
  }
}

} // namespace frontend
