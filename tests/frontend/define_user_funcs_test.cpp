#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("Analyzing user-function definitions", "[frontend]") {

  SECTION("Define basic function") {
    const auto& output = ANALYZE("fun f(int a) -> int 42");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));
    const auto& consts  = funcDef.getConsts();
    auto a              = consts.lookup("a");
    REQUIRE(a);
    CHECK(consts[a.value()].getKind() == prog::sym::ConstKind::Input);
    CHECK(consts[a.value()].getType() == GET_TYPE_ID(output, "int"));
    CHECK(funcDef.getExpr() == *prog::expr::litIntNode(output.getProg(), 42));
  }

  SECTION("Define function with conversion") {
    const auto& output = ANALYZE("fun f() -> float 2");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");

    CHECK(
        funcDef.getExpr() ==
        *applyConv(output, "int", "float", prog::expr::litIntNode(output.getProg(), 2)));
  }

  SECTION("Define templated function") {
    const auto& output = ANALYZE("fun f{T}(T a) -> T a "
                                 "fun f1() f{int}(42)");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f__int", GET_TYPE_ID(output, "int"));
    const auto& consts  = funcDef.getConsts();
    auto a              = consts.lookup("a");
    REQUIRE(a);
    CHECK(consts[a.value()].getKind() == prog::sym::ConstKind::Input);
    CHECK(consts[a.value()].getType() == GET_TYPE_ID(output, "int"));
    CHECK(funcDef.getExpr() == *prog::expr::constExprNode(consts, *a));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f() -> int true",
        errNonMatchingFuncReturnType(src, "f", "int", "bool", input::Span{15, 18}));
    CHECK_DIAG(
        "fun f(int int) -> int true",
        errConstNameConflictsWithType(src, "int", input::Span{10, 12}));
    CHECK_DIAG(
        "fun f(int delegate) -> int true",
        errConstNameConflictsWithType(src, "delegate", input::Span{10, 17}));
    CHECK_DIAG(
        "fun f(int a, int a) -> int true",
        errConstNameConflictsWithConst(src, "a", input::Span{17, 17}));
    CHECK_DIAG("fun f() -> int f2()", errUndeclaredPureFunc(src, "f2", {}, input::Span{15, 18}));
    CHECK_DIAG(
        "fun f() -> int bool(1)",
        errUndeclaredTypeOrConversion(src, "bool", {"int"}, input::Span{15, 21}));
    CHECK_DIAG(
        "fun f() -> int int{float}()",
        errNoTypeOrConversionFoundToInstantiate(src, "int", 1, input::Span{15, 26}));
    CHECK_DIAG(
        "fun f2() -> int f{int}(1)", errNoFuncFoundToInstantiate(src, "f", 1, input::Span{16, 24}));
    CHECK_DIAG(
        "fun f1() -> int 1 "
        "fun f2() -> int f1{int}()",
        errNoFuncFoundToInstantiate(src, "f1", 1, input::Span{34, 42}));
    CHECK_DIAG(
        "fun f{T}(T t) t "
        "fun f2() -> int f{int, float}(1)",
        errNoFuncFoundToInstantiate(src, "f", 2, input::Span{32, 47}));
    CHECK_DIAG(
        "fun f{T}(T T) -> T T "
        "fun f2() -> int f{int}(1)",
        errConstNameConflictsWithTypeSubstitution(src, "T", input::Span{11, 11}),
        errInvalidFuncInstantiation(src, input::Span{37, 37}),
        errNoFuncFoundToInstantiate(src, "f", 1, input::Span{37, 45}));
    CHECK_DIAG(
        "fun f{T}(T T) -> T T "
        "fun f2() f{int}(1)",
        errConstNameConflictsWithTypeSubstitution(src, "T", input::Span{11, 11}),
        errInvalidFuncInstantiation(src, input::Span{30, 30}),
        errNoFuncFoundToInstantiate(src, "f", 1, input::Span{30, 38}));
    CHECK_DIAG(
        "fun f{T}(T i) -> T "
        "  T = i * 2; i "
        "fun f2() -> int f{int}(1)",
        errConstNameConflictsWithTypeSubstitution(src, "T", input::Span{21, 21}),
        errInvalidFuncInstantiation(src, input::Span{50, 50}),
        errNoFuncFoundToInstantiate(src, "f", 1, input::Span{50, 58}));
    CHECK_DIAG(
        "fun f() -> delegate{int} lambda () false",
        errNonMatchingFuncReturnType(
            src, "f", "delegate{int}", "delegate{bool}", input::Span{25, 39}));
    CHECK_DIAG(
        "fun f() -> string print(\"hello world\")",
        errUndeclaredPureFunc(src, "print", {"string"}, input::Span{18, 37}));

    SECTION("Infinite recursion") {
      CHECK_DIAG("fun f() f()", errUnableToInferFuncReturnType(src, "f", input::Span{0, 10}));

      CHECK_DIAG("fun f() -> int f()", errPureFuncInfRecursion(src, input::Span{15, 17}));
      CHECK_DIAG("fun f() -> int a = f()", errPureFuncInfRecursion(src, input::Span{15, 21}));
      CHECK_DIAG("fun f(int i) -> int i + f(i)", errPureFuncInfRecursion(src, input::Span{20, 27}));
      CHECK_DIAG(
          "fun f(int a, int b) -> int "
          " a2 = 42; b2 = 1337; f(a + a2, b + b2)",
          errPureFuncInfRecursion(src, input::Span{28, 64}));
      CHECK_DIAG(
          "fun f(int a, int b) -> int "
          " if f(a, b) == a -> a "
          " else            -> b",
          errPureFuncInfRecursion(src, input::Span{28, 69}));
      CHECK_DIAG(
          "fun f(int a, int b) -> int "
          " if a > b  -> f(a, 0) "
          " else      -> f(0, b)",
          errPureFuncInfRecursion(src, input::Span{28, 69}));
    }
  }
}

} // namespace frontend
