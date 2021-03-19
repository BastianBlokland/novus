#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"
#include "prog/expr/node_lit_int.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyze static integer", "frontend") {

  SECTION("Static integers can be used as types") {
    const auto& output = ANALYZE("fun f(#42 i) -> #42 i ");
    REQUIRE(output.isSuccess());

    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "#42"));
    const auto& consts  = funcDef.getConsts();

    CHECK(funcDef.getBody() == *prog::expr::constExprNode(consts, *consts.lookup("i")));
  }

  SECTION("Static integers can be converted to normal integer literals") {
    const auto& output = ANALYZE("fun f() -> int intrinsic{staticint_to_int}{#42}()");
    REQUIRE(output.isSuccess());

    CHECK(GET_FUNC_DEF(output, "f").getBody() == *prog::expr::litIntNode(output.getProg(), 42));
  }

  SECTION("Non static integer types are converted to -1") {
    const auto& output = ANALYZE("fun f() -> int intrinsic{staticint_to_int}{string}()");
    REQUIRE(output.isSuccess());

    CHECK(GET_FUNC_DEF(output, "f").getBody() == *prog::expr::litIntNode(output.getProg(), -1));
  }
}

} // namespace frontend
