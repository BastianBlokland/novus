#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_float.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("Analyzing user-function templates", "[frontend]") {

  SECTION("Recursive templated call") {
    const auto& output = ANALYZE("fun ft{T}(T a) -> T a != 0 ? ft{T}(a) : a "
                                 "fun f() -> int ft{int}(1)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto fArgs       = std::vector<prog::expr::NodePtr>{};
    fArgs.push_back(prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "ft__int", GET_TYPE_ID(output, "int")),
            std::move(fArgs)));
  }

  SECTION("Chained templated call") {
    const auto& output = ANALYZE("fun ft1{T}(T a, T b) -> T a + b "
                                 "fun ft2{T}(T a) -> T ft1{T}(a, a) "
                                 "fun f() -> int ft2{int}(1)");
    REQUIRE(output.isSuccess());

    const auto& ft2Def = GET_FUNC_DEF(output, "ft2__int", GET_TYPE_ID(output, "int"));
    auto ft2Args       = std::vector<prog::expr::NodePtr>{};
    ft2Args.push_back(
        prog::expr::constExprNode(ft2Def.getConsts(), *ft2Def.getConsts().lookup("a")));
    ft2Args.push_back(
        prog::expr::constExprNode(ft2Def.getConsts(), *ft2Def.getConsts().lookup("a")));
    CHECK(
        ft2Def.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "ft1__int", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "int")),
            std::move(ft2Args)));

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto fArgs       = std::vector<prog::expr::NodePtr>{};
    fArgs.push_back(prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "ft2__int", GET_TYPE_ID(output, "int")),
            std::move(fArgs)));
  }

  SECTION("Overload templated functions") {
    const auto& output = ANALYZE("fun ft{T}(int a, T b) a + b "
                                 "fun ft{T}(float a, T b) a + b "
                                 "fun f() -> float ft{int}(1.0, 2)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");

    auto args = std::vector<prog::expr::NodePtr>{};
    args.push_back(prog::expr::litFloatNode(output.getProg(), 1.0));
    args.push_back(prog::expr::litIntNode(output.getProg(), 2));

    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(
                output, "ft__int", GET_TYPE_ID(output, "float"), GET_TYPE_ID(output, "int")),
            std::move(args)));
  }

  SECTION("Return type as parameter") {
    const auto& output = ANALYZE("fun ft{T}(int a) -> T a "
                                 "fun f1() -> int ft{int}(1) "
                                 "fun f2() -> float ft{float}(1) "
                                 "fun f3() -> string ft{string}(1)");
    REQUIRE(output.isSuccess());

    const auto& f1Def = GET_FUNC_DEF(output, "f1");
    const auto& f2Def = GET_FUNC_DEF(output, "f2");
    const auto& f3Def = GET_FUNC_DEF(output, "f3");

    auto args1 = std::vector<prog::expr::NodePtr>{};
    args1.push_back(prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        f1Def.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "ft__int", GET_TYPE_ID(output, "int")),
            std::move(args1)));

    auto args2 = std::vector<prog::expr::NodePtr>{};
    args2.push_back(prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        f2Def.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "ft__float", GET_TYPE_ID(output, "int")),
            std::move(args2)));

    auto args3 = std::vector<prog::expr::NodePtr>{};
    args3.push_back(prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        f3Def.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "ft__string", GET_TYPE_ID(output, "int")),
            std::move(args3)));
  }

  SECTION("Substituted constructor") {
    const auto& output = ANALYZE("struct s = int a "
                                 "fun factory{T}(int i) -> T T(i) "
                                 "fun f() -> s factory{s}(1)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "factory__s", GET_TYPE_ID(output, "int"));
    auto fArgs       = std::vector<prog::expr::NodePtr>{};
    fArgs.push_back(prog::expr::constExprNode(fDef.getConsts(), *fDef.getConsts().lookup("i")));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "s", GET_TYPE_ID(output, "int")),
            std::move(fArgs)));
  }

  SECTION("Infer type-parameter in templated call") {
    const auto& output = ANALYZE("fun ft{T, Y}(T a, Y b) "
                                 "  a + b "
                                 "fun f() ft(2, 1.0)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto fArgs       = std::vector<prog::expr::NodePtr>{};
    fArgs.push_back(prog::expr::litIntNode(output.getProg(), 2));
    fArgs.push_back(prog::expr::litFloatNode(output.getProg(), 1.0));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(
                output, "ft__int_float", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "float")),
            std::move(fArgs)));
  }

  SECTION("Infer type-parameter in templated call") {
    const auto& output = ANALYZE("struct Null "
                                 "union Option{T} = T, Null "
                                 "fun ft{T}(Option{T} a, Option{T} b) "
                                 "  if a is T aVal && b is T bVal -> aVal + bVal "
                                 "  else -> T()"
                                 "fun f() ft(Option{int}(1), Option{int}(2))");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto fArgs       = std::vector<prog::expr::NodePtr>{};
    fArgs.push_back(
        applyConv(output, "int", "Option__int", prog::expr::litIntNode(output.getProg(), 1)));
    fArgs.push_back(
        applyConv(output, "int", "Option__int", prog::expr::litIntNode(output.getProg(), 2)));

    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(
                output,
                "ft__int",
                GET_TYPE_ID(output, "Option__int"),
                GET_TYPE_ID(output, "Option__int")),
            std::move(fArgs)));
  }

  SECTION("Overload func templates based on amount of type-parameters") {
    const auto& output = ANALYZE("union Choice{T1, T2} = T1, T2 "
                                 "fun ft{T1, T2}(T1 a, T2 b) -> Choice{T1, T2} "
                                 "  bool(a) ? a : b "
                                 "fun ft{T}(T a, T b) -> T "
                                 "  a + b "
                                 "fun f1() ft(1, 2)"
                                 "fun f2() ft(false, 2)");
    REQUIRE(output.isSuccess());

    // Check that f1 instantiates the version with one type-parameter.
    const auto& f1Def = GET_FUNC_DEF(output, "f1");
    auto f1Args       = std::vector<prog::expr::NodePtr>{};
    f1Args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    f1Args.push_back(prog::expr::litIntNode(output.getProg(), 2));
    CHECK(
        f1Def.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(output, "ft__int", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "int")),
            std::move(f1Args)));

    // Check that f2 instantiates the version with two type-parameters.
    const auto& f2Def = GET_FUNC_DEF(output, "f2");
    auto f2Args       = std::vector<prog::expr::NodePtr>{};
    f2Args.push_back(prog::expr::litBoolNode(output.getProg(), false));
    f2Args.push_back(prog::expr::litIntNode(output.getProg(), 2));
    CHECK(
        f2Def.getExpr() ==
        *prog::expr::callExprNode(
            output.getProg(),
            GET_FUNC_ID(
                output, "ft__bool_int", GET_TYPE_ID(output, "bool"), GET_TYPE_ID(output, "int")),
            std::move(f2Args)));
  }
}

} // namespace frontend
