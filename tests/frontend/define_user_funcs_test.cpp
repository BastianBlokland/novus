#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing user-function definitions", "frontend") {

  SECTION("Define basic function") {
    const auto& output = ANALYZE("fun f(int a) -> int 42");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "int"));
    const auto& consts  = funcDef.getConsts();
    auto a              = consts.lookup("a");
    REQUIRE(a);
    CHECK(consts[a.value()].getKind() == prog::sym::ConstKind::Input);
    CHECK(consts[a.value()].getType() == GET_TYPE_ID(output, "int"));
    CHECK(funcDef.getBody() == *prog::expr::litIntNode(output.getProg(), 42));
  }

  SECTION("Define function with conversion") {
    const auto& output = ANALYZE("fun f() -> float 2");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");

    CHECK(
        funcDef.getBody() ==
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
    CHECK(funcDef.getBody() == *prog::expr::constExprNode(consts, *a));
  }

  SECTION("Define noinline function") {
    const auto& output = ANALYZE("fun noinline f() -> int 42");
    REQUIRE(output.isSuccess());
    const auto& funcDef = GET_FUNC_DEF(output, "f");
    CHECK(funcDef.getBody() == *prog::expr::litIntNode(output.getProg(), 42));
    CHECK(funcDef.hasFlags(prog::sym::FuncDef::Flags::NoInline));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("fun f() -> int true", errNonMatchingFuncReturnType(NO_SRC, "f", "int", "bool"));
    CHECK_DIAG("fun f(int int) -> int true", errConstNameConflictsWithType(NO_SRC, "int"));
    CHECK_DIAG(
        "fun f(int function) -> int true", errConstNameConflictsWithType(NO_SRC, "function"));
    CHECK_DIAG("fun f(int action) -> int true", errConstNameConflictsWithType(NO_SRC, "action"));
    CHECK_DIAG("fun f(int a, int a) -> int true", errConstNameConflictsWithConst(NO_SRC, "a"));
    CHECK_DIAG("fun f() -> int f2()", errUndeclaredPureFunc(NO_SRC, "f2", {}));
    CHECK_DIAG("fun f() -> int bool(1)", errUndeclaredTypeOrConversion(NO_SRC, "bool", {"int"}));
    CHECK_DIAG(
        "fun f() -> int int{float}()", errNoTypeOrConversionFoundToInstantiate(NO_SRC, "int", 1));
    CHECK_DIAG("fun f2() -> int f{int}(1)", errNoPureFuncFoundToInstantiate(NO_SRC, "f", 1));
    CHECK_DIAG("act a() -> int f{int}()", errNoFuncOrActionFoundToInstantiate(NO_SRC, "f", 1));
    CHECK_DIAG("f{int}()", errNoFuncOrActionFoundToInstantiate(NO_SRC, "f", 1));
    CHECK_DIAG(
        "fun f1() -> int 1 "
        "fun f2() -> int f1{int}()",
        errNoPureFuncFoundToInstantiate(NO_SRC, "f1", 1));
    CHECK_DIAG(
        "fun f{T}(T t) t "
        "fun f2() -> int f{int, float}(1)",
        errNoPureFuncFoundToInstantiate(NO_SRC, "f", 2));
    CHECK_DIAG(
        "fun f{T}(T T) -> T T "
        "fun f2() -> int f{int}(1)",
        errConstNameConflictsWithTypeSubstitution(NO_SRC, "T"),
        errInvalidFuncInstantiation(NO_SRC),
        errNoPureFuncFoundToInstantiate(NO_SRC, "f", 1));
    CHECK_DIAG(
        "fun f{T}(T T) -> T T "
        "fun f2() f{int}(1)",
        errConstNameConflictsWithTypeSubstitution(NO_SRC, "T"),
        errInvalidFuncInstantiation(NO_SRC),
        errNoPureFuncFoundToInstantiate(NO_SRC, "f", 1));
    CHECK_DIAG(
        "fun f{T}(T i) -> T "
        "  T = i * 2; i "
        "fun f2() -> int f{int}(1)",
        errConstNameConflictsWithTypeSubstitution(NO_SRC, "T"),
        errInvalidFuncInstantiation(NO_SRC),
        errNoPureFuncFoundToInstantiate(NO_SRC, "f", 1));
    CHECK_DIAG(
        "fun f() -> function{int} lambda () false",
        errNonMatchingFuncReturnType(NO_SRC, "f", "function{int}", "function{bool}"));
    CHECK_DIAG(
        "fun f() -> string conWrite(\"hello world\")",
        errUndeclaredPureFunc(NO_SRC, "conWrite", {"string"}));

    SECTION("Infinite recursion") {
      CHECK_DIAG("fun f() f()", errUnableToInferFuncReturnType(NO_SRC, "f"));

      CHECK_DIAG("fun f() -> int f()", errPureFuncInfRecursion(NO_SRC));
      CHECK_DIAG("fun f() -> int a = f()", errPureFuncInfRecursion(NO_SRC));
      CHECK_DIAG("fun f(int i) -> int i + f(i)", errPureFuncInfRecursion(NO_SRC));
      CHECK_DIAG(
          "fun f(int a, int b) -> int "
          " a2 = 42; b2 = 1337; f(a + a2, b + b2)",
          errPureFuncInfRecursion(NO_SRC));
      CHECK_DIAG(
          "fun f(int a, int b) -> int "
          " if f(a, b) == a -> a "
          " else            -> b",
          errPureFuncInfRecursion(NO_SRC));
      CHECK_DIAG(
          "fun f(int a, int b) -> int "
          " if a > b  -> f(a, 0) "
          " else      -> f(0, b)",
          errPureFuncInfRecursion(NO_SRC));
      CHECK_DIAG("fun f(int a = a) a", errUndeclaredConst(NO_SRC, "a"));
      CHECK_DIAG("fun f(int a = b = 42) a", errConstDeclareNotSupported(NO_SRC));
      CHECK_DIAG(
          "union U = int, bool "
          "fun getU() U(42) "
          "fun f(int a = getU() as int i ? i : 0) a",
          errConstDeclareNotSupported(NO_SRC));
    }
  }
}

} // namespace frontend
