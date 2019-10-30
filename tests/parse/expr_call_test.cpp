#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_call.hpp"
#include "parse/node_expr_group.hpp"

namespace parse {

TEST_CASE("Parsing call expressions", "[parse]") {

  CHECK_EXPR("a()", callExprNode(ID("a"), OPAREN, NODES(), COMMAS(0), CPAREN));
  CHECK_EXPR("a(1,2)", callExprNode(ID("a"), OPAREN, NODES(INT(1), INT(2)), COMMAS(1), CPAREN));
  CHECK_EXPR(
      "a(1,2,3+4*5)",
      callExprNode(
          ID("a"),
          OPAREN,
          NODES(INT(1), INT(2), binaryExprNode(INT(3), PLUS, binaryExprNode(INT(4), STAR, INT(5)))),
          COMMAS(2),
          CPAREN));
  CHECK_EXPR(
      "a(1;2,3;4)",
      callExprNode(
          ID("a"),
          OPAREN,
          NODES(
              groupExprNode(NODES(INT(1), INT(2)), SEMIS(1)),
              groupExprNode(NODES(INT(3), INT(4)), SEMIS(1))),
          COMMAS(1),
          CPAREN));
  CHECK_EXPR(
      "a(b())",
      callExprNode(
          ID("a"),
          OPAREN,
          NODES(callExprNode(ID("b"), OPAREN, NODES(), COMMAS(0), CPAREN)),
          COMMAS(0),
          CPAREN));

  SECTION("Errors") {
    CHECK_EXPR(
        "a(1 1)", errInvalidCallExpr(ID("a"), OPAREN, NODES(INT(1), INT(1)), COMMAS(0), CPAREN));
    CHECK_EXPR("a(", errInvalidCallExpr(ID("a"), OPAREN, NODES(), COMMAS(0), END));
    CHECK_EXPR(
        "a(,",
        errInvalidCallExpr(ID("a"), OPAREN, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), END));
    CHECK_EXPR(
        "a(,)",
        callExprNode(ID("a"), OPAREN, NODES(errInvalidPrimaryExpr(COMMA)), COMMAS(0), CPAREN));
  }

  SECTION("Spans") { CHECK_EXPR_SPAN("a(1,2,3+4*5)", input::Span(0, 11)); }
}

} // namespace parse
