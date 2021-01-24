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
    const auto& output = ANALYZE("fun f() b = 1; c = 2; b * c");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");
    const auto& consts  = funcDef.getConsts();

    auto callArgs = std::vector<prog::expr::NodePtr>{};
    callArgs.push_back(prog::expr::constExprNode(consts, consts.lookup("b").value()));
    callArgs.push_back(prog::expr::constExprNode(consts, consts.lookup("c").value()));

    auto exprs = std::vector<prog::expr::NodePtr>{};
    exprs.push_back(prog::expr::assignExprNode(
        consts, consts.lookup("b").value(), prog::expr::litIntNode(output.getProg(), 1)));
    exprs.push_back(prog::expr::assignExprNode(
        consts, consts.lookup("c").value(), prog::expr::litIntNode(output.getProg(), 2)));
    exprs.push_back(prog::expr::callExprNode(
        output.getProg(),
        GET_OP_ID(
            output, prog::Operator::Star, GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "int")),
        std::move(callArgs)));
    auto groupExpr = prog::expr::groupExprNode(std::move(exprs));

    CHECK(funcDef.getBody() == *groupExpr);
  }
}

} // namespace frontend
