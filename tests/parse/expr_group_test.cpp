#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_group.hpp"

namespace parse {

TEST_CASE("Parsing group expressions", "[parse]") {

  CHECK_EXPR("1;2;3", groupExprNode(NODES(INT(1), INT(2), INT(3)), SEMIS(2)));
  CHECK_EXPR(
      "-1;2;3", groupExprNode(NODES(unaryExprNode(MINUS, INT(1)), INT(2), INT(3)), SEMIS(2)));
  CHECK_EXPR(
      "(1;(2;+3))",
      parenExprNode(
          OPAREN,
          groupExprNode(
              NODES(
                  INT(1),
                  parenExprNode(
                      OPAREN,
                      groupExprNode(NODES(INT(2), unaryExprNode(PLUS, INT(3))), SEMIS(1)),
                      CPAREN)),
              SEMIS(1)),
          CPAREN));
  CHECK_EXPR(
      "x == y; 1 == 2",
      groupExprNode(
          NODES(binaryExprNode(CONST("x"), EQEQ, CONST("y")), binaryExprNode(INT(1), EQEQ, INT(2))),
          SEMIS(1)));

  SECTION("Errors") {
    CHECK_EXPR(";", errInvalidUnaryOp(SEMI, errInvalidPrimaryExpr(END)));
    CHECK_EXPR("1;", groupExprNode(NODES(INT(1), errInvalidPrimaryExpr(END)), SEMIS(1)));
  }
}

} // namespace parse
