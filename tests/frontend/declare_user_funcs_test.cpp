#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_call.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/operator.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing user-function declarations", "frontend") {

  SECTION("Pure functions") {

    SECTION("Declare basic function") {
      const auto& output = ANALYZE("fun f(int a, bool b) -> bool false");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "bool"))
              .getOutput() == GET_TYPE_ID(output, "bool"));
    }

    SECTION("Declare type-inferred function") {
      const auto& output = ANALYZE("fun f(int a, bool b) b");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "bool"))
              .getOutput() == GET_TYPE_ID(output, "bool"));
    }

    SECTION("Declare overloaded function") {
      const auto& output = ANALYZE("fun f(int a) -> bool false "
                                   "fun f(string a) -> string \"hello world\" "
                                   "fun f(bool a) -> bool a "
                                   "fun f() -> int 1");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int")).getOutput() ==
          GET_TYPE_ID(output, "bool"));
      CHECK(
          GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "string")).getOutput() ==
          GET_TYPE_ID(output, "string"));
      CHECK(
          GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "bool")).getOutput() ==
          GET_TYPE_ID(output, "bool"));
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == output.getProg().getInt());
    }

    SECTION("Declare function template") {
      const auto& output = ANALYZE("fun f{T}(T a) -> T a "
                                   "fun f2() -> int f{int}(1) "
                                   "fun f3() -> float f{float}(1.0)");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DECL(output, "f__int", GET_TYPE_ID(output, "int")).getOutput() ==
          GET_TYPE_ID(output, "int"));
      CHECK(
          GET_FUNC_DECL(output, "f__float", GET_TYPE_ID(output, "float")).getOutput() ==
          GET_TYPE_ID(output, "float"));
    }

    SECTION("Declare templated conversion function") {
      const auto& output = ANALYZE("struct Tuple{T, Y} = T a, Y b "
                                   "fun string{T, Y}(Tuple{T, Y} t) "
                                   " t.a.string() + \",\" + t.b.string() "
                                   "fun f() string{int, bool}(Tuple{int, bool}(42, false))");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "string"));
      CHECK(
          GET_FUNC_DECL(output, "string", GET_TYPE_ID(output, "Tuple__int_bool")).getOutput() ==
          GET_TYPE_ID(output, "string"));
    }

    SECTION("Declare templated conversion function") {
      const auto& output = ANALYZE("struct Tuple{T, Y} = T a, Y b "
                                   "fun Tuple(int i) Tuple{int, bool}(i, false)"
                                   "fun f() Tuple{int, bool}(42)");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Tuple__int_bool"));
      CHECK(
          GET_FUNC_DECL(output, "Tuple__int_bool", GET_TYPE_ID(output, "int")).getOutput() ==
          GET_TYPE_ID(output, "Tuple__int_bool"));
    }

    SECTION("Declare templated conversion function") {
      const auto& output = ANALYZE("struct Tuple{T, Y} = T a, Y b "
                                   "fun Tuple{T}(T t) Tuple{T, bool}(t, false) "
                                   "fun f() Tuple{string}(\"hello world\")");
      REQUIRE(output.isSuccess());
      CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "Tuple__string_bool"));
      CHECK(
          GET_FUNC_DECL(output, "Tuple__string_bool", GET_TYPE_ID(output, "string")).getOutput() ==
          GET_TYPE_ID(output, "Tuple__string_bool"));
    }

    SECTION("Overload operator") {
      const auto& output = ANALYZE("fun -(bool b) !b "
                                   "fun f() -false");
      REQUIRE(output.isSuccess());

      const auto& funcDef = GET_FUNC_DEF(output, "f");
      CHECK(
          funcDef.getBody() ==
          *prog::expr::callExprNode(
              output.getProg(),
              GET_OP_ID(output, prog::Operator::Minus, GET_TYPE_ID(output, "bool")),
              EXPRS(prog::expr::litBoolNode(output.getProg(), false))));
    }

    SECTION("Declare function with optional argument") {
      const auto& output = ANALYZE("fun f(int a, bool b = false) -> bool false");
      REQUIRE(output.isSuccess());
      const auto& funcDecl =
          GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "bool"));
      CHECK(funcDecl.getNumOptInputs() == 1);
      CHECK(funcDecl.getOutput() == GET_TYPE_ID(output, "bool"));
    }
  }

  SECTION("Actions") {

    SECTION("Declare basic action") {
      const auto& output = ANALYZE("act a(int a, bool b) -> bool false");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DECL(output, "a", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "bool"))
              .getOutput() == GET_TYPE_ID(output, "bool"));
    }

    SECTION("Declare action template") {
      const auto& output = ANALYZE("act a{T}(T a) -> T a "
                                   "act f2() -> int a{int}(1) "
                                   "act f3() -> float a{float}(1.0)");
      REQUIRE(output.isSuccess());
      CHECK(
          GET_FUNC_DECL(output, "a__int", GET_TYPE_ID(output, "int")).getOutput() ==
          GET_TYPE_ID(output, "int"));
      CHECK(
          GET_FUNC_DECL(output, "a__float", GET_TYPE_ID(output, "float")).getOutput() ==
          GET_TYPE_ID(output, "float"));
    }
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun a() -> int 1 "
        "fun a() -> int 1",
        errDuplicateFuncDeclaration(NO_SRC, "a"));
    CHECK_DIAG("fun a(b c) -> int 1", errUndeclaredType(NO_SRC, "b", 0));
    CHECK_DIAG("fun a() -> b 1", errUndeclaredType(NO_SRC, "b", 0));
    CHECK_DIAG("fun bool(int i) -> int i", errIncorrectReturnTypeInConvFunc(NO_SRC, "bool", "int"));
    CHECK_DIAG(
        "struct s{T1, T2} = T1 a, T2 b "
        "fun s{T}(T a) -> T a "
        "fun f() s{int}(1)",
        errIncorrectReturnTypeInConvFunc(NO_SRC, "s{int}", "int"),
        errInvalidFuncInstantiation(NO_SRC),
        errNoTypeOrConversionFoundToInstantiate(NO_SRC, "s", 1));
    CHECK_DIAG("fun -(int i) -> int 1", errDuplicateFuncDeclaration(NO_SRC, "operator-"));
    CHECK_DIAG("fun &&() -> int 1", errNonOverloadableOperator(NO_SRC, "&&"));
    CHECK_DIAG("fun f{int}() -> int 1", errTypeParamNameConflictsWithType(NO_SRC, "int"));
    CHECK_DIAG(
        "fun f{T}(T{int} a) -> int i "
        "fun f() f{int}(1)",
        errTypeParamOnSubstitutionType(NO_SRC, "T"),
        errUndeclaredType(NO_SRC, "T", 1));
    CHECK_DIAG(
        "fun f{T}(T a) b "
        "fun f() f{int}(1)",
        errUndeclaredConst(NO_SRC, "b"),
        errInvalidFuncInstantiation(NO_SRC),
        errNoPureFuncFoundToInstantiate(NO_SRC, "f", 1));
    CHECK_DIAG("fun +() -> int i", errOperatorOverloadWithoutArgs(NO_SRC, "operator+"));
    CHECK_DIAG("fun +{T}() -> T T()", errOperatorOverloadWithoutArgs(NO_SRC, "operator+"));
    CHECK_DIAG("fun f{T}(test a) -> int 1", errUndeclaredType(NO_SRC, "test", 0));
    CHECK_DIAG("fun f{T}(int{M} a) -> int 1", errUndeclaredType(NO_SRC, "M", 0));
    CHECK_DIAG("fun f{T}(int{T{M}} a) -> int 1", errUndeclaredType(NO_SRC, "M", 0));
    CHECK_DIAG(
        "fun fa(bool b, string s) b "
        "fun fa(bool b, string s) b",
        errDuplicateFuncDeclaration(NO_SRC, "fa"));
    CHECK_DIAG("act +(int i) i + 1", errNonPureOperatorOverload(NO_SRC));
    CHECK_DIAG("act +{T}(T t) t + 1", errNonPureOperatorOverload(NO_SRC));
    CHECK_DIAG(
        "struct S = int i, bool b "
        "act S() S(0, false)",
        errNonPureConversion(NO_SRC));
    CHECK_DIAG(
        "struct S{T} = int i, T t "
        "act S{T}() S{T}(0, T()) "
        "act a() S{int}()",
        errNonPureConversion(NO_SRC),
        errInvalidFuncInstantiation(NO_SRC),
        errUndeclaredTypeOrConversion(NO_SRC, "S{int}", {}));
    CHECK_DIAG("fun f(int a = 0, int b) a * b", errNonOptArgFollowingOpt(NO_SRC));
  }
}

} // namespace frontend
