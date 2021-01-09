#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "opt/opt.hpp"

namespace opt {

TEST_CASE("[opt] Optimization synergy", "opt") {

  SECTION("Dynamic calls to function literals are inlined") {

    const auto& output = ANALYZE("fun invoke(function{int, int} func, int arg) func(arg) "
                                 "fun funcB(int i) i "
                                 "act actA() invoke(funcB, 42) "
                                 "actA()");
    REQUIRE(output.isSuccess());

    auto optimizedProg = optimize(output.getProg());

    // Verify that 'funcB' and 'invoke' are both inlined into 'funcA' as expected.
    auto actAId = optimizedProg.lookupFunc("actA", prog::sym::TypeSet{}, prog::OvOptions{});
    REQUIRE(actAId);

    const auto& inlADef = optimizedProg.getFuncDef(*actAId);

    CHECK(inlADef.getExpr() == *prog::expr::litIntNode(optimizedProg, 42));
  }
}

} // namespace opt
