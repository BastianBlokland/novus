#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_comma.hpp"
#include "parse/node_expr_paren.hpp"
#include "parse/node_expr_unary.hpp"

namespace parse {

TEST_CASE("Parsing comma expressions", "[parse]") {

  CHECK_EXPR("1,2,3", COMMA_EXPR(INT(1), INT(2), INT(3)));
  CHECK_EXPR("-1,2,3", COMMA_EXPR(unaryExprNode(MINUS, INT(1)), INT(2), INT(3)));
  CHECK_EXPR(
      "(1,(2,+3))",
      parenExprNode(
          OPAREN,
          COMMA_EXPR(
              INT(1),
              parenExprNode(OPAREN, COMMA_EXPR(INT(2), unaryExprNode(PLUS, INT(3))), CPAREN)),
          CPAREN));
  CHECK_EXPR(
      "x == y, 1 == 2",
      COMMA_EXPR(
          binaryExprNode(CONST("x"), EQEQ, CONST("y")), binaryExprNode(INT(1), EQEQ, INT(2))));

  SECTION("Errors") {
    CHECK_EXPR(",", errInvalidPrimaryExpr(COMMA));
    CHECK_EXPR("1,", COMMA_EXPR(INT(1), errInvalidPrimaryExpr(END)));
  }
}

} // namespace parse
