#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("[frontend] Infer return type of user functions", "frontend") {

  SECTION("Int literal") {
    const auto& output = ANALYZE("fun f() 1");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Long literal") {
    const auto& output = ANALYZE("fun f() 1L");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "long"));
  }

  SECTION("Float literal") {
    const auto& output = ANALYZE("fun f() 1.0");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "float"));
  }

  SECTION("Bool literal") {
    const auto& output = ANALYZE("fun f() false");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("String literal") {
    const auto& output = ANALYZE("fun f() \"hello world\"");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "string"));
  }

  SECTION("Character literal") {
    const auto& output = ANALYZE("fun f() 'a'");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "char"));
  }

  SECTION("Function literal") {
    const auto& output = ANALYZE("fun f1(int i) i "
                                 "fun f() f1");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "__function_int_int"));
  }

  SECTION("Action literal") {
    const auto& output = ANALYZE("act a1(int i) i "
                                 "act a() a1");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "a").getOutput() == GET_TYPE_ID(output, "__action_int_int"));
  }

  SECTION("Templated function literal") {
    const auto& output = ANALYZE("fun f1{T}(T t) t "
                                 "fun f() f1{int}");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "__function_int_int"));
  }

  SECTION("Templated action literal") {
    const auto& output = ANALYZE("act a1{T}(T t) t "
                                 "act a() a1{int}");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "a").getOutput() == GET_TYPE_ID(output, "__action_int_int"));
  }

  SECTION("Function argument") {
    const auto& output = ANALYZE("fun f(int arg) arg");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int")).getOutput() ==
        GET_TYPE_ID(output, "int"));
  }

  SECTION("Parenthesized") {
    const auto& output = ANALYZE("fun f() (1)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Constant") {
    const auto& output = ANALYZE("fun f() x = 45; x");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Field") {
    const auto& output = ANALYZE("struct S = int a "
                                 "fun f(S s) s.a");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "S")).getOutput() ==
        GET_TYPE_ID(output, "int"));
  }

  SECTION("Enum entry") {
    const auto& output = ANALYZE("enum E = a, b "
                                 "fun f() E.a");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "E"));
  }

  SECTION("Logic and operator") {
    const auto& output = ANALYZE("fun f() true && false");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Logic or operator") {
    const auto& output = ANALYZE("fun f() true || false");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Binary operator") {
    const auto& output = ANALYZE("fun +(int x, int y) -> int intrinsic{int_add_int}(x, y) "
                                 "fun f() 42 + 1337");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Unary operator") {
    const auto& output = ANALYZE("fun -(int x) -> int intrinsic{int_neg}(x) "
                                 "fun f() -42");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Conditional operator") {
    const auto& output = ANALYZE("fun implicit float(int i) intrinsic{int_to_float}(i) "
                                 "fun f() intrinsic{int_le_int}(1, 2) ? 42 : 1337.0");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "float"));
  }

  SECTION("Switch") {
    const auto& output = ANALYZE("fun implicit float(int i) intrinsic{int_to_float}(i) "
                                 "fun f() "
                                 "if intrinsic{int_gt_int}(1, 2) -> 1 "
                                 "else                           -> 2.0");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "float"));
  }

  SECTION("Call") {
    const auto& output = ANALYZE("fun f1() false "
                                 "fun f2() f1()");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f2").getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Call function that is declared later") {
    const auto& output = ANALYZE("fun f1() f2() "
                                 "fun f2() false");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f1").getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Recursive call in conditional") {
    const auto& output = ANALYZE("fun f(bool b) b ? f(false) : 42");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "bool")).getOutput() ==
        GET_TYPE_ID(output, "int"));
  }

  SECTION("Recursive call in switch") {
    const auto& output = ANALYZE("fun f(bool b) "
                                 "  if b  -> f(false) "
                                 "  else  -> 42");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "bool")).getOutput() ==
        GET_TYPE_ID(output, "int"));
  }

  SECTION("Templated call") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "fun ft{T}(T a) a == a "
                                 "fun f(int i) ft{int}(i)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int")).getOutput() ==
        GET_TYPE_ID(output, "bool"));
  }

  SECTION("Recursive templated call") {
    const auto& output = ANALYZE("fun ft{T}(T a) intrinsic{int_le_int}(a, 0) ? ft(a) : a "
                                 "fun f(int i) ft{int}(i)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int")).getOutput() ==
        GET_TYPE_ID(output, "int"));
  }

  SECTION("Forked conversion call") {
    const auto& output = ANALYZE("struct s = int i "
                                 "fun s(string str) s(intrinsic{string_length}(str)) "
                                 "fun f2() fork s(\"hello world\")");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f2").getOutput() == GET_TYPE_ID(output, "__future_s"));
  }

  SECTION("Forked call") {
    const auto& output = ANALYZE("fun f1() false "
                                 "fun f2() fork f1()");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f2").getOutput() == GET_TYPE_ID(output, "__future_bool"));
  }

  SECTION("Forked templated call") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "fun ft{T}(T a) a == a "
                                 "fun f(int i) fork ft{int}(i)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int")).getOutput() ==
        GET_TYPE_ID(output, "__future_bool"));
  }

  SECTION("Lazy conversion call") {
    const auto& output = ANALYZE("struct s = int i "
                                 "fun s(string str) s(intrinsic{string_length}(str)) "
                                 "fun f2() lazy s(\"hello world\")");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f2").getOutput() == GET_TYPE_ID(output, "__lazy_s"));
  }

  SECTION("Lazy call") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "fun f1(int i) i == i "
                                 "fun f2() lazy f1(42)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f2").getOutput() == GET_TYPE_ID(output, "__lazy_bool"));
  }

  SECTION("Lazy action call") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "act a1(int i) i == i "
                                 "act a2() lazy a1(42)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "a2").getOutput() == GET_TYPE_ID(output, "__lazy_action_bool"));
  }

  SECTION("Lazy templated call") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "fun ft{T}(T a) a == a "
                                 "fun f(int i) lazy ft{int}(i)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int")).getOutput() ==
        GET_TYPE_ID(output, "__lazy_bool"));
  }

  SECTION("Lazy templated action call") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "act at{T}(T a) a == a "
                                 "act a(int i) lazy at{int}(i)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "a", GET_TYPE_ID(output, "int")).getOutput() ==
        GET_TYPE_ID(output, "__lazy_action_bool"));
  }

  SECTION("Templated constructor") {
    const auto& output = ANALYZE("struct tuple{T1, T2} = T1 a, T2 b "
                                 "fun f(int i) tuple{int, bool}(i, false)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int")).getOutput() ==
        GET_TYPE_ID(output, "tuple__int_bool"));
  }

  SECTION("Substituted constructor") {
    const auto& output = ANALYZE("struct s = int a "
                                 "fun factory{T}(int i) T(i) "
                                 "fun f() factory{s}(42)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "s"));
  }

  SECTION("Index operator") {
    const auto& output = ANALYZE("fun +(int x, int y) -> int intrinsic{int_add_int}(x, y) "
                                 "struct s = int a "
                                 "fun [](s a, int i) a.a + i "
                                 "fun f(s a) a[0]");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "s")).getOutput() ==
        GET_TYPE_ID(output, "int"));
  }

  SECTION("Call operator") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "fun ()(int i) i == 0 "
                                 "fun f() 42()");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Forked call operator") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "fun ()(int i) i == 0 "
                                 "fun f() fork 42()");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "__future_bool"));
  }

  SECTION("Lazy call operator") {
    const auto& output = ANALYZE("fun ()(int x, int y) x * y "
                                 "fun f() lazy 42(1337)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "__lazy_int"));
  }

  SECTION("Instance function call") {
    const auto& output = ANALYZE("fun double(int i) i * 2 "
                                 "fun f() (42).double()");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Instance function call with arguments") {
    const auto& output =
        ANALYZE("fun +(string x, string y) -> string intrinsic{string_add_string}(x, y) "
                "fun string(int i) intrinsic{int_to_string}(i) "
                "fun test(int a, string b) a.string() + b "
                "fun f() (42).test(\"test\")");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "string"));
  }

  SECTION("Dynamic call") {
    const auto& output = ANALYZE("fun f(function{int, int} op) op(1)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "__function_int_int")).getOutput() ==
        GET_TYPE_ID(output, "int"));
  }

  SECTION("Dynamic call on struct field") {
    const auto& output = ANALYZE("struct S = function{int} del "
                                 "fun f(S s) s.del()");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "S")).getOutput() ==
        GET_TYPE_ID(output, "int"));
  }

  SECTION("Forked dynamic call") {
    const auto& output = ANALYZE("fun f(function{int, int} op) fork op(1)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "__function_int_int")).getOutput() ==
        GET_TYPE_ID(output, "__future_int"));
  }

  SECTION("Lazy dynamic call") {
    const auto& output = ANALYZE("fun f(function{int, int} op) lazy op(1)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "__function_int_int")).getOutput() ==
        GET_TYPE_ID(output, "__lazy_int"));
  }

  SECTION("Lazy dynamic action call") {
    const auto& output = ANALYZE("act a(action{int, int} op) lazy op(1)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "a", GET_TYPE_ID(output, "__action_int_int")).getOutput() ==
        GET_TYPE_ID(output, "__lazy_action_int"));
  }

  SECTION("Anonymous function") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "fun f() lambda (int i) i == i");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "__function_int_bool"));
  }

  SECTION("Anonymous function with return type spec") {
    const auto& output = ANALYZE("fun implicit float(int i) intrinsic{int_to_float}(i) "
                                 "fun f() lambda (int i) -> float i");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "__function_int_float"));
  }

  SECTION("Anonymous function call") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "fun f() (lambda (int i) i == i)(42)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Anonymous function with closure") {
    const auto& output =
        ANALYZE("fun +(float x, float y) -> float intrinsic{float_add_float}(x, y) "
                "fun f(float a) lambda (float b) b + a");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "float")).getOutput() ==
        GET_TYPE_ID(output, "__function_float_float"));
  }

  SECTION("Anonymous function with nested closure") {
    const auto& output =
        ANALYZE("fun +(float x, float y) -> float intrinsic{float_add_float}(x, y) "
                "fun f(float a) lambda (float b) (lambda () b + a)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "float")).getOutput() ==
        GET_TYPE_ID(output, "__function_float___function_float"));
  }

  SECTION("Anonymous function call with closure") {
    const auto& output =
        ANALYZE("fun +(float x, float y) -> float intrinsic{float_add_float}(x, y) "
                "fun f(float a) (lambda (float b) b + a)(42.0)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "float")).getOutput() ==
        GET_TYPE_ID(output, "float"));
  }

  SECTION("Anonymous function call with nested closure") {
    const auto& output = ANALYZE("fun +(int x, int y) -> int intrinsic{int_add_int}(x, y) "
                                 "fun f(int a) (lambda (int b) (lambda () b + a))(42)");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int")).getOutput() ==
        GET_TYPE_ID(output, "__function_int"));
  }

  SECTION("Anonymous action") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "act a() impure lambda (int i) i == i");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "a").getOutput() == GET_TYPE_ID(output, "__action_int_bool"));
  }

  SECTION("Anonymous action call") {
    const auto& output = ANALYZE("fun ==(int x, int y) -> bool intrinsic{int_eq_int}(x, y) "
                                 "act a() (lambda (int i) i == i)(42)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "a").getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Templated overloaded call") {
    const auto& output = ANALYZE("fun f1{T}(T a, int b) f1(a, false) "
                                 "fun f1{T}(T a, bool b) b "
                                 "fun f2() f1(0, 0)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f2").getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f1() f2() "
        "fun f2() f1()",
        errUnableToInferFuncReturnType(NO_SRC, "f1"));
  }
}

} // namespace frontend
