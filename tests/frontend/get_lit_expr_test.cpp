#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_bool.hpp"
#include "prog/expr/node_lit_int.hpp"
#include "prog/operator.hpp"

namespace frontend {

TEST_CASE("Get literal expression", "[frontend]") {

  SECTION("Get int literal expression") {
    const auto& output = ANALYZE("fun f() -> int 42");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f").getExpr() == *prog::expr::litIntNode(output.getProg(), 42));
  }

  SECTION("Get int literal expression") {
    const auto& output = ANALYZE("fun f() -> bool true");
    REQUIRE(output.isSuccess());
    CHECK(GET_FUNC_DEF(output, "f").getExpr() == *prog::expr::litBoolNode(output.getProg(), true));
  }
}

} // namespace frontend