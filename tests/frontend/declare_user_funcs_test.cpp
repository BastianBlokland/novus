#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/sym/input.hpp"

namespace frontend {

TEST_CASE("Declare user functions", "[frontend]") {

  SECTION("Declare basic function") {
    const auto& output = ANALYZE("fun f(int a, bool b) -> bool false");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int"), GET_TYPE_ID(output, "bool"))
            .getSig()
            .getOutput() == GET_TYPE_ID(output, "bool"));
  }

  SECTION("Declare overloaded function") {
    const auto& output = ANALYZE("fun f(int a) -> bool false "
                                 "fun f(bool a) -> bool a "
                                 "fun f() -> int 1");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "int")).getSig().getOutput() ==
        GET_TYPE_ID(output, "bool"));
    CHECK(
        GET_FUNC_DECL(output, "f", GET_TYPE_ID(output, "bool")).getSig().getOutput() ==
        GET_TYPE_ID(output, "bool"));
    CHECK(GET_FUNC_DECL(output, "f").getSig().getOutput() == output.getProg().lookupType("int"));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun int() -> bool true", errFuncNameConflictsWithType(src, "int", input::Span{4, 6}));
    CHECK_DIAG(
        "fun print() -> int 1", errFuncNameConflictsWithAction(src, "print", input::Span{4, 8}));
    CHECK_DIAG(
        "fun a() -> int 1 "
        "fun a() -> int 1",
        errDuplicateFuncDeclaration(src, "a", input::Span{17, 32}));
    CHECK_DIAG("fun a(b c) -> int 1", errUndeclaredType(src, "b", input::Span{6, 6}));
    CHECK_DIAG("fun a() -> b 1", errUndeclaredType(src, "b", input::Span{11, 11}));
  }
}

} // namespace frontend
