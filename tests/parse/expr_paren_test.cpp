#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"

namespace parse {

TEST_CASE("Parsing parenthesized expressions", "[parse]") {

  CHECK_EXPR(
      "-(-1)", unaryExprNode(MINUS, parenExprNode(OPAREN, unaryExprNode(MINUS, INT(1)), CPAREN)));
  CHECK_EXPR("((1))", parenExprNode(OPAREN, parenExprNode(OPAREN, INT(1), CPAREN), CPAREN));
  CHECK_EXPR(
      "(1 + 2) * 5",
      binaryExprNode(
          parenExprNode(OPAREN, binaryExprNode(INT(1), PLUS, INT(2)), CPAREN), STAR, INT(5)));

  SECTION("Errors") {
    CHECK_EXPR("(1", errInvalidParenExpr(OPAREN, INT(1), END));
    CHECK_EXPR("(", errInvalidParenExpr(OPAREN, errInvalidPrimaryExpr(END), END));
  }

  SECTION("Spans") { CHECK_EXPR_SPAN("(1 + 2) * 5", lex::SourceSpan(0, 10)); }
}

} // namespace parse
