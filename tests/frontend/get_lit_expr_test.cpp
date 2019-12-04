#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_float.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/expr/node_lit_string.hpp"
#include "prog/operator.hpp"

namespace frontend {

TEST_CASE("Analyzing literal expressions", "[frontend]") {

  SECTION("Get int literal expression") {
    const auto& output = ANALYZE("fun f() -> int 42");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f").getExpr() == *prog::expr::litIntNode(output.getProg(), 42));
  }

  SECTION("Get float literal expression") {
    const auto& output = ANALYZE("fun f() -> float 42.0");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DEF(output, "f").getExpr() ==
        *prog::expr::litFloatNode(output.getProg(), 42.0F)); // NOLINT: Magic numbers
  }

  SECTION("Get bool literal expression") {
    const auto& output = ANALYZE("fun f() -> bool true");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f").getExpr() == *prog::expr::litBoolNode(output.getProg(), true));
  }

  SECTION("Get string literal expression") {
    const auto& output = ANALYZE("fun f() -> string \"hello world\"");
    REQUIRE(output.isSuccess());
    CHECK(
        GET_FUNC_DEF(output, "f").getExpr() ==
        *prog::expr::litStringNode(output.getProg(), "hello world"));
  }
}

} // namespace frontend
