#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_conditional.hpp"
#include "parse/node_expr_group.hpp"

namespace parse {

TEST_CASE("Parsing conditional expressions", "[parse]") {

  CHECK_EXPR("x ? y : z", conditionalExprNode(CONST("x"), QMARK, CONST("y"), COLON, CONST("z")));
  CHECK_EXPR(
      "x ? y : z ? a : b",
      conditionalExprNode(
          CONST("x"),
          QMARK,
          CONST("y"),
          COLON,
          conditionalExprNode(CONST("z"), QMARK, CONST("a"), COLON, CONST("b"))));
  CHECK_EXPR(
      "x ? a; b : z",
      conditionalExprNode(
          CONST("x"),
          QMARK,
          groupExprNode(NODES(CONST("a"), CONST("b")), SEMIS(1)),
          COLON,
          CONST("z")));

  SECTION("Errors") {
    CHECK_EXPR(
        "x ?",
        errInvalidConditionalExpr(
            CONST("x"), QMARK, errInvalidPrimaryExpr(END), END, errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "x ? y",
        errInvalidConditionalExpr(CONST("x"), QMARK, CONST("y"), END, errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "x ? y :",
        conditionalExprNode(CONST("x"), QMARK, CONST("y"), COLON, errInvalidPrimaryExpr(END)));
  }

  SECTION("Spans") { CHECK_EXPR_SPAN("x ? y : z", input::Span(0, 8)); }
}

} // namespace parse
