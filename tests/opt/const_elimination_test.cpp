#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "opt/opt.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace opt {

TEST_CASE("Constants elimination", "[opt]") {

  SECTION("Eliminate one-time use constants") {

    const auto& output = ANALYZE("fun func() a = 42; b = 1337; a + b");
    REQUIRE(output.isSuccess());

    auto optProg = eliminateConsts(output.getProg());

    // Verify that both constants are removed.
    const auto& funcDef   = GET_FUNC_DEF(optProg, "func");
    const auto intAddFunc = GET_OP_FUNC_ID(
        optProg, prog::Operator::Plus, GET_TYPE_ID(optProg, "int"), GET_TYPE_ID(optProg, "int"));

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 42));   // NOLINT: Magic numbers
    args.push_back(prog::expr::litIntNode(output.getProg(), 1337)); // NOLINT: Magic numbers
    auto callExpr = prog::expr::callExprNode(optProg, intAddFunc, std::move(args));

    CHECK(funcDef.getExpr() == *callExpr);
  }

  SECTION("Trivial constants are eliminated") {

    const auto& output = ANALYZE("fun func() a = 42; a + a");
    REQUIRE(output.isSuccess());

    auto optProg = eliminateConsts(output.getProg());

    // Verify that both constants are removed.
    const auto& funcDef   = GET_FUNC_DEF(optProg, "func");
    const auto intAddFunc = GET_OP_FUNC_ID(
        optProg, prog::Operator::Plus, GET_TYPE_ID(optProg, "int"), GET_TYPE_ID(optProg, "int"));

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litIntNode(output.getProg(), 42)); // NOLINT: Magic numbers
    args.push_back(prog::expr::litIntNode(output.getProg(), 42)); // NOLINT: Magic numbers
    auto callExpr = prog::expr::callExprNode(optProg, intAddFunc, std::move(args));

    CHECK(funcDef.getExpr() == *callExpr);
  }
}

} // namespace opt
