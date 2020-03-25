#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "opt/opt.hpp"
#include "prog/expr/node_assign.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_group.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace opt {

TEST_CASE("Call inline", "[opt]") {

  SECTION("Inline basic call") {

    const auto& output = ANALYZE("fun funcA() funcB(42) "
                                 "fun funcB(int i) i");
    REQUIRE(output.isSuccess());

    auto inlinedProg = inlineCalls(output.getProg());

    // Verify that 'funcB' was inlined into 'funcA' as expected.
    auto funcBId = inlinedProg.lookupFunc("funcA", prog::sym::TypeSet{}, prog::OvOptions{});
    REQUIRE(funcBId);

    const auto& inlBDef = inlinedProg.getFuncDef(*funcBId);
    auto inlBExprs      = std::vector<prog::expr::NodePtr>{};

    inlBExprs.push_back(prog::expr::assignExprNode(
        inlBDef.getConsts(),
        *inlBDef.getConsts().lookup("__inlined_0_i"),
        prog::expr::litIntNode(inlinedProg, 42))); // NOLINT: Magic numbers

    inlBExprs.push_back(prog::expr::constExprNode(
        inlBDef.getConsts(), *inlBDef.getConsts().lookup("__inlined_0_i")));

    CHECK(inlBDef.getExpr() == *prog::expr::groupExprNode(std::move(inlBExprs)));
  }

  SECTION("Inline nested call") {

    const auto& output = ANALYZE("fun funcA() funcB() "
                                 "fun funcB() funcC(42) "
                                 "fun funcC(int i) i");
    REQUIRE(output.isSuccess());

    auto inlinedProg = inlineCalls(output.getProg());

    // Verify that 'funcB' and 'funcC' was inlined into 'funcA' as expected.
    auto funcBId = inlinedProg.lookupFunc("funcA", prog::sym::TypeSet{}, prog::OvOptions{});
    REQUIRE(funcBId);

    const auto& inlBDef = inlinedProg.getFuncDef(*funcBId);
    auto inlBExprs      = std::vector<prog::expr::NodePtr>{};

    inlBExprs.push_back(prog::expr::assignExprNode(
        inlBDef.getConsts(),
        *inlBDef.getConsts().lookup("__inlined_0_i"),
        prog::expr::litIntNode(inlinedProg, 42))); // NOLINT: Magic numbers

    inlBExprs.push_back(prog::expr::constExprNode(
        inlBDef.getConsts(), *inlBDef.getConsts().lookup("__inlined_0_i")));

    CHECK(inlBDef.getExpr() == *prog::expr::groupExprNode(std::move(inlBExprs)));
  }

  SECTION("Inline nested call in arg") {

    const auto& output = ANALYZE("fun funcA() funcC(funcB()) "
                                 "fun funcB() 42 "
                                 "fun funcC(int i) i");
    REQUIRE(output.isSuccess());

    auto inlinedProg = inlineCalls(output.getProg());

    // Verify that 'funcB' and 'funcC' was inlined into 'funcA' as expected.
    auto funcBId = inlinedProg.lookupFunc("funcA", prog::sym::TypeSet{}, prog::OvOptions{});
    REQUIRE(funcBId);

    const auto& inlBDef = inlinedProg.getFuncDef(*funcBId);
    auto inlBExprs      = std::vector<prog::expr::NodePtr>{};

    inlBExprs.push_back(prog::expr::assignExprNode(
        inlBDef.getConsts(),
        *inlBDef.getConsts().lookup("__inlined_0_i"),
        prog::expr::litIntNode(inlinedProg, 42))); // NOLINT: Magic numbers

    inlBExprs.push_back(prog::expr::constExprNode(
        inlBDef.getConsts(), *inlBDef.getConsts().lookup("__inlined_0_i")));

    CHECK(inlBDef.getExpr() == *prog::expr::groupExprNode(std::move(inlBExprs)));
  }
}

} // namespace opt
