#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call_dyn.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_func.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("Analyzing call dynamic expressions", "[frontend]") {

  SECTION("Get delegate call without args") {
    const auto& output = ANALYZE("fun f1() -> int 1 "
                                 "fun f2(func{int} op) -> int op() "
                                 "fun f() -> int f2(f1)");
    REQUIRE(output.isSuccess());
    const auto& f2Def = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "__func_int"));
    const auto& fDef  = GET_FUNC_DEF(output, "f");

    CHECK(
        f2Def.getExpr() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(f2Def.getConsts(), *f2Def.getConsts().lookup("op")),
            {}));

    auto fArgs = std::vector<prog::expr::NodePtr>{};
    fArgs.push_back(prog::expr::litFuncNode(
        output.getProg(), GET_TYPE_ID(output, "__func_int"), GET_FUNC_ID(output, "f1")));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(output.getProg(), f2Def.getId(), std::move(fArgs)));
  }

  SECTION("Get delegate call with args") {
    const auto& output = ANALYZE("fun f1(bool b, float v) -> int b ? 1 : 0 "
                                 "fun f2(func{bool, float, int} op) -> int op(false, 1) "
                                 "fun f() -> int f2(f1)");
    REQUIRE(output.isSuccess());
    const auto& f2Def = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "__func_bool_float_int"));
    const auto& fDef  = GET_FUNC_DEF(output, "f");

    auto f2Args = std::vector<prog::expr::NodePtr>{};
    f2Args.push_back(prog::expr::litBoolNode(output.getProg(), false));
    f2Args.push_back(prog::expr::litIntNode(output.getProg(), 1));
    CHECK(
        f2Def.getExpr() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(f2Def.getConsts(), *f2Def.getConsts().lookup("op")),
            std::move(f2Args)));

    auto fArgs = std::vector<prog::expr::NodePtr>{};
    fArgs.push_back(prog::expr::litFuncNode(
        output.getProg(),
        GET_TYPE_ID(output, "__func_bool_float_int"),
        GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "bool"), GET_TYPE_ID(output, "float"))));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(output.getProg(), f2Def.getId(), std::move(fArgs)));
  }

  SECTION("Get delegate call with templated function") {
    const auto& output = ANALYZE("fun f1{T}() -> T T() "
                                 "fun f2(func{int} op) -> int op() "
                                 "fun f() -> int f2(f1{int})");
    REQUIRE(output.isSuccess());
    const auto& f2Def = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "__func_int"));
    const auto& fDef  = GET_FUNC_DEF(output, "f");

    CHECK(
        f2Def.getExpr() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(f2Def.getConsts(), *f2Def.getConsts().lookup("op")),
            {}));

    auto fArgs = std::vector<prog::expr::NodePtr>{};
    fArgs.push_back(prog::expr::litFuncNode(
        output.getProg(), GET_TYPE_ID(output, "__func_int"), GET_FUNC_ID(output, "f1__int")));
    CHECK(
        fDef.getExpr() ==
        *prog::expr::callExprNode(output.getProg(), f2Def.getId(), std::move(fArgs)));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f(func{int, float, bool} op) -> bool op(false, 1.0)",
        errIncorrectArgsToDelegate(src, input::Span{41, 54}));
    CHECK_DIAG(
        "fun f1(int v) -> int v "
        "fun f1(float v) -> float v "
        "fun f2() -> int op = f1; op()",
        errAmbiguousFunction(src, "f1", input::Span{71, 72}));
    CHECK_DIAG(
        "fun f1{T}(int v) -> T T() "
        "fun f1{T}(float v) -> T T() "
        "fun f2() -> int op = f1{int}; op(1)",
        errAmbiguousTemplateFunction(src, "f1", 1, input::Span{75, 81}));
    CHECK_DIAG("fun f() f1{float}", errNoFuncFoundToInstantiate(src, "f1", 1, input::Span{8, 16}));
    CHECK_DIAG(
        "fun f1{T}() -> T T() "
        "fun f2() -> int op = f1; op()",
        errNoTypeParamsProvidedToTemplateFunction(src, "f1", input::Span{42, 43}));
  }
}

} // namespace frontend
