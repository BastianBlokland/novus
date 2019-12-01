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

  SECTION("Diagnostics") {
    CHECK_DIAG("struct int = bool i", errTypeAlreadyDeclared(src, "int", input::Span{7, 9}));
    CHECK_DIAG(
        "struct print = bool i", errTypeNameConflictsWithAction(src, "print", input::Span{7, 11}));
    CHECK_DIAG(
        "struct s = int i "
        "struct s = bool b",
        errTypeAlreadyDeclared(src, "s", input::Span{24, 24}));
  }
}

} // namespace frontend
