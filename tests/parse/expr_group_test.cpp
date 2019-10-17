#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"

namespace parse {

TEST_CASE("Parsing group expressions", "[parse]") {

  CHECK_EXPR("1;2;3", GROUP_EXPR(INT(1), INT(2), INT(3)));
  CHECK_EXPR("-1;2;3", GROUP_EXPR(unaryExprNode(MINUS, INT(1)), INT(2), INT(3)));
  CHECK_EXPR(
      "(1;(2;+3))",
      parenExprNode(
          OPAREN,
          GROUP_EXPR(
              INT(1),
              parenExprNode(OPAREN, GROUP_EXPR(INT(2), unaryExprNode(PLUS, INT(3))), CPAREN)),
          CPAREN));
  CHECK_EXPR(
      "x == y; 1 == 2",
      GROUP_EXPR(
          binaryExprNode(CONST("x"), EQEQ, CONST("y")), binaryExprNode(INT(1), EQEQ, INT(2))));

  SECTION("Errors") {
    CHECK_EXPR(";", errInvalidUnaryOp(SEMI, errInvalidPrimaryExpr(END)));
    CHECK_EXPR("1;", GROUP_EXPR(INT(1), errInvalidPrimaryExpr(END)));
  }
}

} // namespace parse
