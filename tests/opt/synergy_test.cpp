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

    CHECK(inlADef.getBody() == *prog::expr::litIntNode(optimizedProg, 42));
  }

  SECTION("One time used lazy calls are optimized out") {

    const auto& output =
        ANALYZE("act get{T}(lazy_action{T} a) -> T intrinsic{lazy_action_get}(a) "
                "act getNonZero(lazy_action{int} a, lazy_action{int} b) "
                " v = a.get(); intrinsic{int_gt_int}(v, 0) ? v : b.get() "
                "act produceA() -> int intrinsic{int_neg}(1337) "
                "act produceB(int multiplier) -> int intrinsic{int_mul_int}(42, multiplier) "
                "act main() getNonZero(lazy produceA(), lazy produceB(2)) "
                "main()");
    REQUIRE(output.isSuccess());

    auto optimizedProg = optimize(output.getProg());

    // Verify that everything was precomputed and main just returns a integer literal.
    auto mainId = optimizedProg.lookupFunc("main", prog::sym::TypeSet{}, prog::OvOptions{});
    REQUIRE(mainId);

    const auto& mainDef = optimizedProg.getFuncDef(*mainId);

    CHECK(mainDef.getBody() == *prog::expr::litIntNode(optimizedProg, 42 * 2));
  }
}

} // namespace opt
