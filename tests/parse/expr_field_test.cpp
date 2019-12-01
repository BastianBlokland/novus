#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_field.hpp"
#include "parse/node_expr_paren.hpp"
#include "parse/node_expr_unary.hpp"

namespace parse {

TEST_CASE("Parsing field expressions", "[parse]") {

  CHECK_EXPR("x.a", fieldExprNode(CONST("x"), DOT, ID("a")));
  CHECK_EXPR("-x.a", unaryExprNode(MINUS, fieldExprNode(CONST("x"), DOT, ID("a"))));
  CHECK_EXPR(
      "(x + y).a",
      fieldExprNode(
          parenExprNode(OPAREN, binaryExprNode(CONST("x"), PLUS, CONST("y")), CPAREN),
          DOT,
          ID("a")));
  CHECK_EXPR("x * y.a", binaryExprNode(CONST("x"), STAR, fieldExprNode(CONST("y"), DOT, ID("a"))));

  SECTION("Errors") { CHECK_EXPR("x.+", errInvalidFieldExpr(CONST("x"), DOT, PLUS)); }

  SECTION("Spans") { CHECK_EXPR_SPAN("x.a", input::Span(0, 2)); }
}

} // namespace parse
