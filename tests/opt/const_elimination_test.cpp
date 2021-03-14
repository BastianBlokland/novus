#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "opt/opt.hpp"

namespace opt {

TEST_CASE("[opt] Constants elimination", "opt") {

  SECTION("Eliminate one-time use constants") {

    const auto& output =
        ANALYZE("fun func() -> int a = 42; b = 1337; intrinsic{int_add_int}(a, b)");
    REQUIRE(output.isSuccess());

    auto optProg = eliminateConsts(output.getProg());

    // Verify that both constants are removed.
    const auto& funcDef = GET_FUNC_DEF(optProg, "func");

    CHECK(funcDef.getBody() == *getIntrinsicIntBinaryOp(optProg, "int_add_int", 42, 1337));
  }

  SECTION("Trivial constants are eliminated") {

    const auto& output = ANALYZE("fun func() -> int a = 42; intrinsic{int_add_int}(a, a)");
    REQUIRE(output.isSuccess());

    auto optProg = eliminateConsts(output.getProg());

    // Verify that both constants are removed.
    const auto& funcDef = GET_FUNC_DEF(optProg, "func");

    CHECK(funcDef.getBody() == *getIntrinsicIntBinaryOp(optProg, "int_add_int", 42, 42));
  }
}

} // namespace opt
