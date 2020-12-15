#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "opt/opt.hpp"
#include "prog/expr/node_assign.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_group.hpp"

namespace opt {

TEST_CASE("Call inline", "[opt]") {

  SECTION("Inline basic call") {

    const auto& output = ANALYZE("fun funcA() funcB(42) "
                                 "fun funcB(int i) i");
    REQUIRE(output.isSuccess());

    auto inlinedProg = inlineCalls(output.getProg());

    // Verify that 'funcB' was inlined into 'funcA' as expected.
    auto funcAId = inlinedProg.lookupFunc("funcA", prog::sym::TypeSet{}, prog::OvOptions{});
    REQUIRE(funcAId);

    const auto& inlADef = inlinedProg.getFuncDef(*funcAId);
    auto inlAExprs      = std::vector<prog::expr::NodePtr>{};

    inlAExprs.push_back(prog::expr::assignExprNode(
        inlADef.getConsts(),
        *inlADef.getConsts().lookup("__inlined_0_i"),
        prog::expr::litIntNode(inlinedProg, 42))); // NOLINT: Magic numbers

    inlAExprs.push_back(prog::expr::constExprNode(
        inlADef.getConsts(), *inlADef.getConsts().lookup("__inlined_0_i")));
    auto inlAGroupExpr = prog::expr::groupExprNode(std::move(inlAExprs));

    CHECK(inlADef.getExpr() == *inlAGroupExpr);
  }

  SECTION("Inline nested call") {

    const auto& output = ANALYZE("fun funcA() funcB() "
                                 "fun funcB() funcC(42) "
                                 "fun funcC(int i) i");
    REQUIRE(output.isSuccess());

    auto inlinedProg = inlineCalls(output.getProg());

    // Verify that 'funcB' and 'funcC' was inlined into 'funcA' as expected.
    auto funcAId = inlinedProg.lookupFunc("funcA", prog::sym::TypeSet{}, prog::OvOptions{});
    REQUIRE(funcAId);

    const auto& inlADef = inlinedProg.getFuncDef(*funcAId);
    auto inlAExprs      = std::vector<prog::expr::NodePtr>{};

    inlAExprs.push_back(prog::expr::assignExprNode(
        inlADef.getConsts(),
        *inlADef.getConsts().lookup("__inlined_0_i"),
        prog::expr::litIntNode(inlinedProg, 42))); // NOLINT: Magic numbers

    inlAExprs.push_back(prog::expr::constExprNode(
        inlADef.getConsts(), *inlADef.getConsts().lookup("__inlined_0_i")));
    auto inlAGroupExpr = prog::expr::groupExprNode(std::move(inlAExprs));

    CHECK(inlADef.getExpr() == *inlAGroupExpr);
  }

  SECTION("Inline nested call in arg") {

    const auto& output = ANALYZE("fun funcA() funcC(funcB()) "
                                 "fun funcB() 42 "
                                 "fun funcC(int i) i");
    REQUIRE(output.isSuccess());

    auto inlinedProg = inlineCalls(output.getProg());

    // Verify that 'funcB' and 'funcC' was inlined into 'funcA' as expected.
    auto funcAId = inlinedProg.lookupFunc("funcA", prog::sym::TypeSet{}, prog::OvOptions{});
    REQUIRE(funcAId);

    const auto& inlADef = inlinedProg.getFuncDef(*funcAId);
    auto inlAExprs      = std::vector<prog::expr::NodePtr>{};

    inlAExprs.push_back(prog::expr::assignExprNode(
        inlADef.getConsts(),
        *inlADef.getConsts().lookup("__inlined_0_i"),
        prog::expr::litIntNode(inlinedProg, 42))); // NOLINT: Magic numbers

    inlAExprs.push_back(prog::expr::constExprNode(
        inlADef.getConsts(), *inlADef.getConsts().lookup("__inlined_0_i")));
    auto inlAGroupExpr = prog::expr::groupExprNode(std::move(inlAExprs));

    CHECK(inlADef.getExpr() == *inlAGroupExpr);
  }
}

} // namespace opt
