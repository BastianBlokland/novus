#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call_dyn.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_field.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_func.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing call dynamic expressions", "frontend") {

  SECTION("Get delegate call without args") {
    const auto& output = ANALYZE("fun f1() -> int 1 "
                                 "fun f2(function{int} op) -> int op() "
                                 "fun f() -> int f2(f1)");
    REQUIRE(output.isSuccess());
    const auto& f2Def = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "__function_int"));
    const auto& fDef  = GET_FUNC_DEF(output, "f");

    CHECK(
        f2Def.getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(f2Def.getConsts(), *f2Def.getConsts().lookup("op")),
            {}));

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        f2Def.getId(),
        EXPRS(prog::expr::litFuncNode(
            output.getProg(), GET_TYPE_ID(output, "__function_int"), GET_FUNC_ID(output, "f1"))));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Get delegate call with args") {
    const auto& output = ANALYZE("fun f1(bool b, float v) -> int b ? 1 : 0 "
                                 "fun f2(function{bool, float, int} op) -> int op(false, 1) "
                                 "fun f() -> int f2(f1)");
    REQUIRE(output.isSuccess());
    const auto& f2Def =
        GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "__function_bool_float_int"));
    const auto& fDef = GET_FUNC_DEF(output, "f");

    auto callExpr2 = prog::expr::callDynExprNode(
        output.getProg(),
        prog::expr::constExprNode(f2Def.getConsts(), *f2Def.getConsts().lookup("op")),
        EXPRS(
            prog::expr::litBoolNode(output.getProg(), false),
            prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(f2Def.getBody() == *callExpr2);

    auto callExpr1 = prog::expr::callExprNode(
        output.getProg(),
        f2Def.getId(),
        EXPRS(prog::expr::litFuncNode(
            output.getProg(),
            GET_TYPE_ID(output, "__function_bool_float_int"),
            GET_FUNC_ID(output, "f1", GET_TYPE_ID(output, "bool"), GET_TYPE_ID(output, "float")))));

    CHECK(fDef.getBody() == *callExpr1);
  }

  SECTION("Get delegate call with templated function") {
    const auto& output = ANALYZE("fun f1{T}() -> T T() "
                                 "fun f2(function{int} op) -> int op() "
                                 "fun f() -> int f2(f1{int})");
    REQUIRE(output.isSuccess());
    const auto& f2Def = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "__function_int"));
    const auto& fDef  = GET_FUNC_DEF(output, "f");

    CHECK(
        f2Def.getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(f2Def.getConsts(), *f2Def.getConsts().lookup("op")),
            {}));

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        f2Def.getId(),
        EXPRS(prog::expr::litFuncNode(
            output.getProg(),
            GET_TYPE_ID(output, "__function_int"),
            GET_FUNC_ID(output, "f1__int"))));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Get delegate call on struct") {
    const auto& output = ANALYZE("struct S = function{int} del "
                                 "fun f(S s) -> int s.del()");
    REQUIRE(output.isSuccess());
    const auto& sDef = std::get<prog::sym::StructDef>(GET_TYPE_DEF(output, "S"));
    const auto& fDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "S"));

    CHECK(
        fDef.getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::fieldExprNode(
                output.getProg(),
                prog::expr::constExprNode(fDef.getConsts(), *fDef.getConsts().lookup("s")),
                *sDef.getFields().lookup("del")),
            {}));
  }

  SECTION("Get forked delegate call") {
    const auto& output = ANALYZE("fun f1() -> int 1 "
                                 "fun f2(function{int} op) -> future{int} fork op() "
                                 "fun f() -> future{int} f2(f1)");
    REQUIRE(output.isSuccess());
    const auto& f2Def = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "__function_int"));
    const auto& fDef  = GET_FUNC_DEF(output, "f");

    CHECK(
        f2Def.getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(f2Def.getConsts(), *f2Def.getConsts().lookup("op")),
            GET_TYPE_ID(output, "__future_int"),
            {},
            prog::expr::CallMode::Forked));

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        f2Def.getId(),
        EXPRS(prog::expr::litFuncNode(
            output.getProg(), GET_TYPE_ID(output, "__function_int"), GET_FUNC_ID(output, "f1"))));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Get lazy delegate call") {
    const auto& output = ANALYZE("fun f1() -> int 1 "
                                 "fun f2(function{int} op) -> lazy{int} lazy op() "
                                 "fun f() -> lazy{int} f2(f1)");
    REQUIRE(output.isSuccess());
    const auto& f2Def = GET_FUNC_DEF(output, "f2", GET_TYPE_ID(output, "__function_int"));
    const auto& fDef  = GET_FUNC_DEF(output, "f");

    CHECK(
        f2Def.getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(f2Def.getConsts(), *f2Def.getConsts().lookup("op")),
            GET_TYPE_ID(output, "__lazy_int"),
            {},
            prog::expr::CallMode::Lazy));

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        f2Def.getId(),
        EXPRS(prog::expr::litFuncNode(
            output.getProg(), GET_TYPE_ID(output, "__function_int"), GET_FUNC_ID(output, "f1"))));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Get lazy action delegate call") {
    const auto& output = ANALYZE("act a1() -> int 1 "
                                 "act a2(action{int} op) -> lazy_action{int} lazy op() "
                                 "act a() -> lazy_action{int} a2(a1)");
    REQUIRE(output.isSuccess());
    const auto& a2Def = GET_FUNC_DEF(output, "a2", GET_TYPE_ID(output, "__action_int"));
    const auto& aDef  = GET_FUNC_DEF(output, "a");

    CHECK(
        a2Def.getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(a2Def.getConsts(), *a2Def.getConsts().lookup("op")),
            GET_TYPE_ID(output, "__lazy_action_int"),
            {},
            prog::expr::CallMode::Lazy));

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        a2Def.getId(),
        EXPRS(prog::expr::litFuncNode(
            output.getProg(), GET_TYPE_ID(output, "__action_int"), GET_FUNC_ID(output, "a1"))));

    CHECK(aDef.getBody() == *callExpr);
  }

  SECTION("Implicitly convert function to action") {
    const auto& output = ANALYZE("fun f1() -> int 1 "
                                 "act a2(action{int} op) -> int op() "
                                 "act a1() -> int a2(f1)");
    REQUIRE(output.isSuccess());
    const auto& a2Def = GET_FUNC_DEF(output, "a2", GET_TYPE_ID(output, "__action_int"));
    const auto& a1Def = GET_FUNC_DEF(output, "a1");

    CHECK(
        a2Def.getBody() ==
        *prog::expr::callDynExprNode(
            output.getProg(),
            prog::expr::constExprNode(a2Def.getConsts(), *a2Def.getConsts().lookup("op")),
            {}));

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        a2Def.getId(),
        EXPRS(applyConv(
            output,
            "__function_int",
            "__action_int",
            prog::expr::litFuncNode(
                output.getProg(),
                GET_TYPE_ID(output, "__function_int"),
                GET_FUNC_ID(output, "f1")))));

    CHECK(a1Def.getBody() == *callExpr);
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f(function{int, float, bool} op) -> bool op(false, 1.0)",
        errIncorrectArgsToDelegate(src, input::Span{45, 58}));
    CHECK_DIAG(
        "fun f1(int v) -> int v "
        "fun f1(float v) -> float v "
        "fun f2() -> int op = f1; op()",
        errAmbiguousFunction(src, "f1", input::Span{71, 72}),
        errUndeclaredPureFunc(src, "op", {}, input::Span{75, 78}));
    CHECK_DIAG(
        "fun f1{T}(int v) -> T T() "
        "fun f1{T}(float v) -> T T() "
        "fun f2() -> int op = f1{int}; op(1)",
        errAmbiguousTemplateFunction(src, "f1", 1, input::Span{75, 81}),
        errUndeclaredPureFunc(src, "op", {"int"}, input::Span{84, 88}));
    CHECK_DIAG(
        "fun f() f1{float}", errNoPureFuncFoundToInstantiate(src, "f1", 1, input::Span{8, 16}));
    CHECK_DIAG(
        "act a() f1{float}", errNoFuncOrActionFoundToInstantiate(src, "f1", 1, input::Span{8, 16}));
    CHECK_DIAG(
        "fun f1{T}() -> T T() "
        "fun f2() -> int op = f1; op()",
        errNoTypeParamsProvidedToTemplateFunction(src, "f1", input::Span{42, 43}),
        errUndeclaredPureFunc(src, "op", {}, input::Span{46, 49}));
    CHECK_DIAG(
        "fun f() -> function{string, string} conWrite",
        errUndeclaredConst(src, "conWrite", input::Span{36, 43}));
    CHECK_DIAG(
        "fun f() -> string op = conWrite; op(\"hello world\")",
        errUndeclaredConst(src, "conWrite", input::Span{23, 30}),
        errUndeclaredPureFunc(src, "op", {"string"}, input::Span{33, 49}));
    CHECK_DIAG("fun f(action{int} a) a()", errIllegalDelegateCall(src, input::Span{21, 23}));
    CHECK_DIAG(
        "fun f(action{int} a) lambda () a()", errIllegalDelegateCall(src, input::Span{31, 33}));
  }
}

} // namespace frontend
