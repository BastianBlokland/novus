#include "catch2/catch.hpp"
#include "frontend/diag_defs.hpp"
#include "helpers.hpp"
#include "prog/expr/node_lit_enum.hpp"

namespace frontend {

TEST_CASE("[frontend] Analyzing enum expressions", "frontend") {

  SECTION("Get enum entry") {
    const auto& output = ANALYZE("enum E = a, b "
                                 "fun f() E.a");
    REQUIRE(output.isSuccess());

    CHECK(
        GET_FUNC_DEF(output, "f").getBody() ==
        *prog::expr::litEnumNode(output.getProg(), GET_TYPE_ID(output, "E"), "a"));
  }

  SECTION("Diagnostics") {
    CHECK_DIAG(
        "fun f() -> int int.a", errStaticFieldNotFoundOnType(src, "a", "int", input::Span{19, 19}));
    CHECK_DIAG(
        "enum E = a, c "
        "fun f() -> int E.b",
        errValueNotFoundInEnum(src, "b", "E", input::Span{31, 31}));
  }
}

} // namespace frontend
