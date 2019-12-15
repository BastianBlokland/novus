#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_const.hpp"
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
}

} // namespace frontend
