#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_assign.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_group.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/operator.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing group expressions", "frontend") {

  SECTION("Get basic group expression") {
    const auto& output = ANALYZE("fun *(int x, int y) -> int intrinsic{int_mul_int}(x, y) "
                                 "fun f() b = 1; c = 2; b * c");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");
    const auto& consts  = funcDef.getConsts();

    auto groupExpr = prog::expr::groupExprNode(EXPRS(
        prog::expr::assignExprNode(
            consts, consts.lookup("b").value(), prog::expr::litIntNode(output.getProg(), 1)),
        prog::expr::assignExprNode(
            consts, consts.lookup("c").value(), prog::expr::litIntNode(output.getProg(), 2)),
        prog::expr::callExprNode(
            output.getProg(),
            GET_OP_ID(
                output,
                prog::Operator::Star,
                GET_TYPE_ID(output, "int"),
                GET_TYPE_ID(output, "int")),
            EXPRS(
                prog::expr::constExprNode(consts, consts.lookup("b").value()),
                prog::expr::constExprNode(consts, consts.lookup("c").value())))));

    CHECK(funcDef.getBody() == *groupExpr);
  }
}

} // namespace frontend
