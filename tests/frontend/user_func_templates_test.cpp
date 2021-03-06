#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_float.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing user-function templates", "frontend") {

  SECTION("Recursive templated call") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "fun ft{T}(T a) -> T a == 0 ? ft{T}(a) : a "
                                 "fun f() -> int ft{int}(1)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "ft__int", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Chained templated call") {
    const auto& output = ANALYZE("fun ft1{T}(T a, T b) -> T intrinsic{int_add_int}(a, b) "
                                 "fun ft2{T}(T a) -> T ft1{T}(a, a) "
                                 "fun f() -> int ft2{int}(1)");
    REQUIRE(output.isSuccess());

    const auto& ft2Def = GET_FUNC_DEF(output, "ft2__int", GET_TYPE_ID(output, "int"));
    auto callExpr2     = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "ft1__int", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "int")),
        EXPRS(
            prog::expr::constExprNode(ft2Def.getConsts(), *ft2Def.getConsts().lookup("a")),
            prog::expr::constExprNode(ft2Def.getConsts(), *ft2Def.getConsts().lookup("a"))));

    CHECK(ft2Def.getBody() == *callExpr2);

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "ft2__int", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Overload templated functions") {
    const auto& output =
        ANALYZE("fun +(float x, float y) -> float intrinsic{float_add_float}(x, y) "
                "fun implicit float(int i) intrinsic{int_to_float}(i) "
                "fun ft{T}(int a, T b) -> int intrinsic{int_add_int}(a, b) "
                "fun ft{T}(float a, T b) -> float a + b "
                "fun f() -> float ft{int}(1.0, 2)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");

    auto callExpr = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "ft__int", GET_TYPE_ID(output, "float"), GET_TYPE_ID(output, "int")),
        EXPRS(
            prog::expr::litFloatNode(output.getProg(), 1.0),
            prog::expr::litIntNode(output.getProg(), 2)));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Return type as parameter") {
    const auto& output = ANALYZE("fun implicit float(int i) intrinsic{int_to_float}(i) "
                                 "fun string(int i) intrinsic{int_to_string}(i) "
                                 "fun int(int i) i "
                                 "fun ft{T}(int a) -> T T(a) "
                                 "fun f1() -> int ft{int}(1) "
                                 "fun f2() -> float ft{float}(1) "
                                 "fun f3() -> string ft{string}(1)");
    REQUIRE(output.isSuccess());

    const auto& f1Def = GET_FUNC_DEF(output, "f1");
    const auto& f2Def = GET_FUNC_DEF(output, "f2");
    const auto& f3Def = GET_FUNC_DEF(output, "f3");

    auto callExpr1 = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "ft__int", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(f1Def.getBody() == *callExpr1);

    auto callExpr2 = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "ft__float", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(f2Def.getBody() == *callExpr2);

    auto callExpr3 = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "ft__string", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::litIntNode(output.getProg(), 1)));

    CHECK(f3Def.getBody() == *callExpr3);
  }

  SECTION("Substituted constructor") {
    const auto& output = ANALYZE("struct s = int a "
                                 "fun factory{T}(int i) -> T T(i) "
                                 "fun f() -> s factory{s}(1)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "factory__s", GET_TYPE_ID(output, "int"));
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "s", GET_TYPE_ID(output, "int")),
        EXPRS(prog::expr::constExprNode(fDef.getConsts(), *fDef.getConsts().lookup("i"))));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Infer type-parameter in templated call") {
    const auto& output =
        ANALYZE("fun +(float x, float y) -> float intrinsic{float_add_float}(x, y) "
                "fun implicit float(int i) intrinsic{int_to_float}(i) "
                "fun ft{T, Y}(T a, Y b) "
                "  a + b "
                "fun f() ft(2, 1.0)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(
            output, "ft__int_float", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "float")),
        EXPRS(
            prog::expr::litIntNode(output.getProg(), 2),
            prog::expr::litFloatNode(output.getProg(), 1.0)));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Infer type-parameter supports implicit conversion") {
    const auto& output =
        ANALYZE("fun +(float x, float y) -> float intrinsic{float_add_float}(x, y) "
                "fun implicit float(int i) intrinsic{int_to_float}(i) "
                "fun ft{T}(T a, T b) "
                "  a + b "
                "fun f() ft(1.0, 2)");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(
            output, "ft__float", GET_TYPE_ID(output, "float"), GET_TYPE_ID(output, "float")),
        EXPRS(
            prog::expr::litFloatNode(output.getProg(), 1.0),
            prog::expr::litIntNode(output.getProg(), 2)));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Infer type-parameter in templated call") {
    const auto& output = ANALYZE("fun +(int x, int y) -> int intrinsic{int_add_int}(x, y) "
                                 "fun int() 0 "
                                 "struct Null "
                                 "union Option{T} = T, Null "
                                 "fun ft{T}(Option{T} a, Option{T} b) "
                                 "  if a as T aVal && b as T bVal -> aVal + bVal "
                                 "  else -> T()"
                                 "fun f() ft(Option{int}(1), Option{int}(2))");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(
            output,
            "ft__int",
            GET_TYPE_ID(output, "Option__int"),
            GET_TYPE_ID(output, "Option__int")),
        EXPRS(
            applyConv(output, "int", "Option__int", prog::expr::litIntNode(output.getProg(), 1)),
            applyConv(output, "int", "Option__int", prog::expr::litIntNode(output.getProg(), 2))));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Templated function with non-matching type-parameters is not instantiated") {
    const auto& output = ANALYZE("struct A{T} = T v1, int v2 "
                                 "struct B{T} = T v1, bool v2 "
                                 "fun ft{T}(A{T} a) a / \"doesnotwork\" "
                                 "fun ft{T}(B{T} b) b.v2 "
                                 "fun f() ft(B(42, false))");
    REQUIRE(output.isSuccess());
    CHECK(output.getProg().lookupFunc("ft__int", prog::OvOptions{0}).size() == 1);
  }

  SECTION("Overloaded func templates prefer simpler matches") {
    const auto& output = ANALYZE("struct Null "
                                 "union Option{T} = T, Null "
                                 "fun ft{T}(T a) a / \"doesnotwork\" "
                                 "fun ft{T}(Option{T} a) a "
                                 "fun f() ft(Option{int}(42))");
    REQUIRE(output.isSuccess());

    const auto& fDef = GET_FUNC_DEF(output, "f");
    auto callExpr    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "ft__int", GET_TYPE_ID(output, "Option__int")),
        EXPRS(
            applyConv(output, "int", "Option__int", prog::expr::litIntNode(output.getProg(), 42))));

    CHECK(fDef.getBody() == *callExpr);
  }

  SECTION("Overloaded func templates prefer less type-parameters") {
    const auto& output = ANALYZE("fun +(int x, int y) -> int intrinsic{int_add_int}(x, y) "
                                 "fun bool(bool b) b "
                                 "union Choice{T1, T2} = T1, T2 "
                                 "fun ft{T1, T2}(T1 a, T2 b) -> Choice{T1, T2} "
                                 "  bool(a) ? a : b "
                                 "fun ft{T}(T a, T b) -> T "
                                 "  a + b "
                                 "fun f1() ft(1, 2)"
                                 "fun f2() ft(false, 2)");
    REQUIRE(output.isSuccess());

    // Check that f1 instantiates the version with one type-parameter.
    const auto& f1Def = GET_FUNC_DEF(output, "f1");
    auto callExpr1    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(output, "ft__int", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "int")),
        EXPRS(
            prog::expr::litIntNode(output.getProg(), 1),
            prog::expr::litIntNode(output.getProg(), 2)));

    CHECK(f1Def.getBody() == *callExpr1);

    // Check that f2 instantiates the version with two type-parameters.
    const auto& f2Def = GET_FUNC_DEF(output, "f2");
    auto callExpr2    = prog::expr::callExprNode(
        output.getProg(),
        GET_FUNC_ID(
            output, "ft__bool_int", GET_TYPE_ID(output, "bool"), GET_TYPE_ID(output, "int")),
        EXPRS(
            prog::expr::litBoolNode(output.getProg(), false),
            prog::expr::litIntNode(output.getProg(), 2)));

    CHECK(f2Def.getBody() == *callExpr2);
  }

  SECTION("Func templates are ignored when arguments don't match") {
    const auto& output = ANALYZE("fun ft{T}(int i, T v) -> bool false "
                                 "fun ft{T1, T2}(function{T1} d, T2 v) -> bool false "
                                 "fun f() -> bool ft(lambda () 42, 42)");
    REQUIRE(output.isSuccess());

    // Check that the version of ft with 2 type params is instantiated.
    CHECK(FUNC_EXISTS(output, "ft__int_int"));

    // Check that the version of ft with 1 type params is not instantiated.
    CHECK(!FUNC_EXISTS(output, "ft__int"));
  }

  SECTION("Call conversion function through type substitution") {
    const auto& output = ANALYZE("fun int() 0 "
                                 "struct Test{T} = T val "
                                 "fun Test{T}() Test(T()) "
                                 "fun factory{T}() T() "
                                 "fun f1() factory{Test{int}}()");
    REQUIRE(output.isSuccess());

    const auto& factoryDef = GET_FUNC_DEF(output, "factory__Test__int");
    CHECK(
        factoryDef.getBody() ==
        *prog::expr::callExprNode(output.getProg(), GET_FUNC_ID(output, "Test__int"), {}));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun ft{T}(T a = T(), T b) a * b "
        "fun f() ft(1, 2)",
        errNonOptArgFollowingOpt(NO_SRC));
    CHECK_DIAG(
        "struct S = int i "
        "fun ft{T}() T() "
        "fun f() ft{S}()",
        errUndeclaredTypeOrConversion(NO_SRC, "S", {}),
        errInvalidFuncInstantiation(NO_SRC),
        errNoPureFuncFoundToInstantiate(NO_SRC, "ft", {"S"}));
    CHECK_DIAG(
        "struct S{T} = T t "
        "fun ft{T}() T() "
        "fun f() ft{S{int}}()",
        errUndeclaredTypeOrConversion(NO_SRC, "S{int}", {}),
        errInvalidFuncInstantiation(NO_SRC),
        errNoPureFuncFoundToInstantiate(NO_SRC, "ft", {"S{int}"}));
  }
}

} // namespace frontend
