#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "prog/expr/node_const.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyze static integer", "frontend") {

  SECTION("Static integers can be used as types") {
    const auto& output = ANALYZE("fun f(#42 i) -> #42 i ");
    REQUIRE(output.isSuccess());

    const auto& funcDef = GET_FUNC_DEF(output, "f", GET_TYPE_ID(output, "#42"));
    const auto& consts  = funcDef.getConsts();

    CHECK(funcDef.getBody() == *prog::expr::constExprNode(consts, *consts.lookup("i")));
  }
}

} // namespace frontend
