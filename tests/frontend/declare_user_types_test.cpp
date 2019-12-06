#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("Analyzing user-type declarations", "[frontend]") {

  SECTION("Declare basic struct") {
    const auto& output = ANALYZE("struct s = int i");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "s"));
  }

  SECTION("Declare basic union") {
    const auto& output = ANALYZE("union u = int, float");
    REQUIRE(output.isSuccess());
    CHECK(TYPE_EXISTS(output, "u"));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG("struct int = bool i", errTypeAlreadyDeclared(src, "int", input::Span{7, 9}));
    CHECK_DIAG("union int = int, bool", errTypeAlreadyDeclared(src, "int", input::Span{6, 8}));
    CHECK_DIAG(
        "struct print = bool i", errTypeNameConflictsWithAction(src, "print", input::Span{7, 11}));
    CHECK_DIAG(
        "union print = int, bool",
        errTypeNameConflictsWithAction(src, "print", input::Span{6, 10}));
    CHECK_DIAG(
        "struct s = int i "
        "struct s = bool b",
        errTypeAlreadyDeclared(src, "s", input::Span{24, 24}));
    CHECK_DIAG(
        "union u = int, bool "
        "union u = bool, int",
        errTypeAlreadyDeclared(src, "u", input::Span{26, 26}));
    CHECK_DIAG(
        "union s = int, bool "
        "struct s = bool b",
        errTypeAlreadyDeclared(src, "s", input::Span{27, 27}));
  }
}

} // namespace frontend
