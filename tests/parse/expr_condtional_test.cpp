#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_conditional.hpp"
#include "parse/node_expr_group.hpp"

namespace parse {

TEST_CASE("[parse] Parsing conditional expressions", "parse") {

  CHECK_EXPR(
      "x ? y : z", conditionalExprNode(ID_EXPR("x"), QMARK, ID_EXPR("y"), COLON, ID_EXPR("z")));
  CHECK_EXPR(
      "x ? y : z ? a : b",
      conditionalExprNode(
          ID_EXPR("x"),
          QMARK,
          ID_EXPR("y"),
          COLON,
          conditionalExprNode(ID_EXPR("z"), QMARK, ID_EXPR("a"), COLON, ID_EXPR("b"))));
  CHECK_EXPR(
      "x ? (a; b) : z",
      conditionalExprNode(
          ID_EXPR("x"),
          QMARK,
          parenExprNode(OPAREN, groupExprNode(NODES(ID_EXPR("a"), ID_EXPR("b")), SEMIS(1)), CPAREN),
          COLON,
          ID_EXPR("z")));

  SECTION("Errors") {
    CHECK_EXPR(
        "x ?",
        errInvalidConditionalExpr(
            ID_EXPR("x"), QMARK, errInvalidPrimaryExpr(END), END, errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "x ? y",
        errInvalidConditionalExpr(
            ID_EXPR("x"), QMARK, ID_EXPR("y"), END, errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "x ? y :",
        conditionalExprNode(ID_EXPR("x"), QMARK, ID_EXPR("y"), COLON, errInvalidPrimaryExpr(END)));
  }

  SECTION("Spans") { CHECK_EXPR_SPAN("x ? y : z", input::Span(0, 8)); }
}

} // namespace parse
