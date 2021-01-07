#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_call.hpp"
#include "parse/node_expr_intrinsic.hpp"

namespace parse {

TEST_CASE("Parsing intrinsic expressions", "[parse]") {

  CHECK_EXPR("intrinsic{test}", intrinsicExprNode(INTRINSIC, OCURLY, ID("test"), CCURLY));
  CHECK_EXPR("intrinsic {test}", intrinsicExprNode(INTRINSIC, OCURLY, ID("test"), CCURLY));
  CHECK_EXPR(
      "intrinsic{test}()",
      callExprNode(
          {},
          intrinsicExprNode(INTRINSIC, OCURLY, ID("test"), CCURLY),
          OPAREN,
          NODES(),
          COMMAS(0),
          CPAREN));
  CHECK_EXPR(
      "intrinsic{test}(1,2)",
      callExprNode(
          {},
          intrinsicExprNode(INTRINSIC, OCURLY, ID("test"), CCURLY),
          OPAREN,
          NODES(INT(1), INT(2)),
          COMMAS(1),
          CPAREN));
  CHECK_EXPR(
      "intrinsic {test} (1,2)",
      callExprNode(
          {},
          intrinsicExprNode(INTRINSIC, OCURLY, ID("test"), CCURLY),
          OPAREN,
          NODES(INT(1), INT(2)),
          COMMAS(1),
          CPAREN));

  SECTION("Errors") {
    CHECK_EXPR("intrinsic{}", errInvalidIntrinsicExpr(INTRINSIC, OCURLY, CCURLY, END));
    CHECK_EXPR("intrinsic", errInvalidIntrinsicExpr(INTRINSIC, END, END, END));
    CHECK_EXPR("intrinsic{test", errInvalidIntrinsicExpr(INTRINSIC, OCURLY, ID("test"), END));
    CHECK_EXPR("intrinsic{a,", errInvalidIntrinsicExpr(INTRINSIC, OCURLY, ID("a"), COMMA));
    CHECK_EXPR("intrinsic(test)", errInvalidIntrinsicExpr(INTRINSIC, OPAREN, ID("test"), CPAREN));
    CHECK_EXPR("intrinsic{1}", errInvalidIntrinsicExpr(INTRINSIC, OCURLY, INT_TOK(1), CCURLY));
  }

  SECTION("Spans") {
    CHECK_EXPR_SPAN("intrinsic{test}", input::Span(0, 14));
    CHECK_EXPR_SPAN("intrinsic {test}", input::Span(0, 15));
  }
}

} // namespace parse
