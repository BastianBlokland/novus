#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/sym/input.hpp"

namespace frontend {

TEST_CASE("Infer return type of user functions", "[frontend]") {

  SECTION("Int literal") {
    const auto& output = ANALYZE("fun f() 1");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getSig().getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Bool literal") {
    const auto& output = ANALYZE("fun f() false");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getSig().getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Function argument") {
    const auto& output = ANALYZE("fun f(int arg) arg");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int")).getSig().getOutput() ==
        GET_TYPE_ID(output, "int"));
  }

  SECTION("Parenthesized") {
    const auto& output = ANALYZE("fun f() (1)");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getSig().getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Constant") {
    const auto& output = ANALYZE("fun f() x = 45; x");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getSig().getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Logic and operator") {
    const auto& output = ANALYZE("fun f() true && false");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getSig().getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Logic or operator") {
    const auto& output = ANALYZE("fun f() true || false");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getSig().getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Binary operator") {
    const auto& output = ANALYZE("fun f() 42 + 1337");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getSig().getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Unary operator") {
    const auto& output = ANALYZE("fun f() -42");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getSig().getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Conditional operator") {
    const auto& output = ANALYZE("fun f() 1 > 2 ? 42 : 1337");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getSig().getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Switch") {
    const auto& output = ANALYZE("fun f() "
                                 "if 1 > 2  -> 1 "
                                 "else      -> 2");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f").getSig().getOutput() == GET_TYPE_ID(output, "int"));
  }

  SECTION("Call") {
    const auto& output = ANALYZE("fun f1() false "
                                 "fun f2() f1()");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f2").getSig().getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Call function that is declared later") {
    const auto& output = ANALYZE("fun f1() f2() "
                                 "fun f2() false");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DECL(output, "f1").getSig().getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f1() f2() "
        "fun f2() f1()",
        errUnableToInferFuncReturnType(src, "f1", input::Span{9, 12}),
        errUnableToInferFuncReturnType(src, "f2", input::Span{23, 26}));
    CHECK_DIAG("fun f() a", errUndeclaredConst(src, "a", input::Span{8, 8}));
    CHECK_DIAG(
        "fun f1() f1() "
        "fun f2() a",
        errUnableToInferFuncReturnType(src, "f1", input::Span{9, 12}),
        errUndeclaredConst(src, "a", input::Span{23, 23}));
    CHECK_DIAG(
        "fun f1() f2(); 1 "
        "fun f2() a",
        errUndeclaredConst(src, "a", input::Span{26, 26}));
    CHECK_DIAG(
        "fun f1() a "
        "fun f2() f1() + 1",
        errUndeclaredConst(src, "a", input::Span{9, 9}));
    CHECK_DIAG(
        "fun f1() a "
        "fun f2() 1 + f1()",
        errUndeclaredConst(src, "a", input::Span{9, 9}));
    CHECK_DIAG(
        "fun f1() a "
        "fun f2() -f1()",
        errUndeclaredConst(src, "a", input::Span{9, 9}));
    CHECK_DIAG(
        "fun f1() a "
        "fun f2() f3(f1()) "
        "fun f3(int i) 1",
        errUndeclaredConst(src, "a", input::Span{9, 9}));
  }
}

} // namespace frontend
