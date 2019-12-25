#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_index.hpp"
#include <optional>

namespace parse {

TEST_CASE("Parsing index expressions", "[parse]") {

  CHECK_EXPR("a[1]", indexExprNode(CONST("a"), OSQUARE, NODES(INT(1)), COMMAS(0), CSQUARE));
  CHECK_EXPR(
      "a[1,2]", indexExprNode(CONST("a"), OSQUARE, NODES(INT(1), INT(2)), COMMAS(1), CSQUARE));
  CHECK_EXPR(
      "a[1,2,3+4*5]",
      indexExprNode(
          CONST("a"),
          OSQUARE,
          NODES(INT(1), INT(2), binaryExprNode(INT(3), PLUS, binaryExprNode(INT(4), STAR, INT(5)))),
          COMMAS(2),
          CSQUARE));
  CHECK_EXPR(
      "a[1;2,3;4]",
      indexExprNode(
          CONST("a"),
          OSQUARE,
          NODES(
              groupExprNode(NODES(INT(1), INT(2)), SEMIS(1)),
              groupExprNode(NODES(INT(3), INT(4)), SEMIS(1))),
          COMMAS(1),
          CSQUARE));
  CHECK_EXPR(
      "(a + b)[1,2]",
      indexExprNode(
          parenExprNode(OPAREN, binaryExprNode(CONST("a"), PLUS, CONST("b")), CPAREN),
          OSQUARE,
          NODES(INT(1), INT(2)),
          COMMAS(1),
          CSQUARE));

  SECTION("Errors") {
    // Note: The error on '[]' is an unfortunate side-effect of how we lex '[]' vs '[ ]'.
    CHECK_EXPR("a[]", CONST("a"), errInvalidUnaryOp(SQUARESQUARE, errInvalidPrimaryExpr(END)));
    CHECK_EXPR("a[ ]", errInvalidIndexExpr(CONST("a"), OSQUARE, NODES(), COMMAS(0), CSQUARE));
    CHECK_EXPR(
        "a[1 1]",
        errInvalidIndexExpr(CONST("a"), OSQUARE, NODES(INT(1), INT(1)), COMMAS(0), CSQUARE));
    CHECK_EXPR("a[", errInvalidIndexExpr(CONST("a"), OSQUARE, NODES(), COMMAS(0), END));
    CHECK_EXPR(
        "a[,",
        errInvalidIndexExpr(
            CONST("a"), OSQUARE, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), END));
    CHECK_EXPR(
        "a[,]",
        indexExprNode(
            CONST("a"), OSQUARE, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), CSQUARE));
  }

  SECTION("Spans") {
    CHECK_EXPR_SPAN("a[1,2,3+4*5]", input::Span(0, 11));
    CHECK_EXPR_SPAN(" a[1]", input::Span(1, 4));
  }
}

} // namespace parse
