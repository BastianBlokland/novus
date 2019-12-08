#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/sym/input.hpp"

namespace frontend {

TEST_CASE("Analyzing user-function declarations", "[frontend]") {

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

  SECTION("Declare conversion function") {
    const auto& output = ANALYZE("fun bool(int i) i != 0");
    REQUIRE(output.isSuccess());
    REQUIRE(GET_CONV(output, "int", "bool"));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun print() -> int 1", errFuncNameConflictsWithAction(src, "print", input::Span{4, 8}));
    CHECK_DIAG(
        "fun a() -> int 1 "
        "fun a() -> int 1",
        errDuplicateFuncDeclaration(src, "a", input::Span{17, 32}));
    CHECK_DIAG("fun a(b c) -> int 1", errUndeclaredType(src, "b", input::Span{6, 6}));
    CHECK_DIAG("fun a() -> b 1", errUndeclaredType(src, "b", input::Span{11, 11}));
    CHECK_DIAG(
        "fun bool(int i) -> int i",
        errConvFuncCannotSpecifyReturnType(src, "bool", input::Span{4, 7}));
  }
}

} // namespace frontend
