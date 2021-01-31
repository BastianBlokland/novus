#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "opt/opt.hpp"

namespace opt {

TEST_CASE("[opt] Constants elimination", "opt") {

  SECTION("Eliminate one-time use constants") {

    const auto& output = ANALYZE("fun func() a = 42; b = 1337; a + b");
    REQUIRE(output.isSuccess());

    auto optProg = eliminateConsts(output.getProg());

    // Verify that both constants are removed.
    const auto& funcDef = GET_FUNC_DEF(optProg, "func");

    CHECK(funcDef.getBody() == *getIntBinaryOpExpr(optProg, prog::Operator::Plus, 42, 1337));
  }

  SECTION("Trivial constants are eliminated") {

    const auto& output = ANALYZE("fun func() a = 42; a + a");
    REQUIRE(output.isSuccess());

    auto optProg = eliminateConsts(output.getProg());

    // Verify that both constants are removed.
    const auto& funcDef = GET_FUNC_DEF(optProg, "func");

    CHECK(funcDef.getBody() == *getIntBinaryOpExpr(optProg, prog::Operator::Plus, 42, 42));
  }
}

} // namespace opt
