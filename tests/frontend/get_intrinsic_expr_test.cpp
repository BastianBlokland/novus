#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"

namespace frontend {

TEST_CASE("Analyzing intrinsic expressions", "[frontend]") {

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun test() "
        "intrinsic{test}",
        errIntrinsicFuncLiteral(src, input::Span{11, 25}));
    CHECK_DIAG(
        "fun test() -> int "
        "intrinsic{test}",
        errIntrinsicFuncLiteral(src, input::Span{18, 32}));
  }
}

} // namespace frontend
