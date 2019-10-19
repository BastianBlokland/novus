#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace parse {

TEST_CASE("Parsing binary operators", "[parse]") {

  CHECK_EXPR("-1 + 2", binaryExprNode(unaryExprNode(MINUS, INT(1)), PLUS, INT(2)));
  CHECK_EXPR("y == false", binaryExprNode(CONST("y"), EQEQ, BOOL(false)));
  CHECK_EXPR("1 + 2 + 3", binaryExprNode(binaryExprNode(INT(1), PLUS, INT(2)), PLUS, INT(3)));
  CHECK_EXPR(
      "1 + 3 1 * 2", binaryExprNode(INT(1), PLUS, INT(3)), binaryExprNode(INT(1), STAR, INT(2)));

  SECTION("Precedence") {
    CHECK_EXPR("1 + 2 * 5", binaryExprNode(INT(1), PLUS, binaryExprNode(INT(2), STAR, INT(5))));
    CHECK_EXPR("2 * 5 + 1", binaryExprNode(binaryExprNode(INT(2), STAR, INT(5)), PLUS, INT(1)));
    CHECK_EXPR(
        "-1 * -2",
        binaryExprNode(unaryExprNode(MINUS, INT(1)), STAR, unaryExprNode(MINUS, INT(2))));
    CHECK_EXPR(
        "1 + 2 == 2 + 3",
        binaryExprNode(
            binaryExprNode(INT(1), PLUS, INT(2)), EQEQ, binaryExprNode(INT(2), PLUS, INT(3))));
    CHECK_EXPR(
        "x || y && -z == q > -5 - 2 * 3",
        binaryExprNode(
            CONST("x"),
            PIPEPIPE,
            binaryExprNode(
                CONST("y"),
                AMPAMP,
                binaryExprNode(
                    unaryExprNode(MINUS, CONST("z")),
                    EQEQ,
                    binaryExprNode(
                        CONST("q"),
                        GT,
                        binaryExprNode(
                            unaryExprNode(MINUS, INT(5)),
                            MINUS,
                            binaryExprNode(INT(2), STAR, INT(3))))))));
  }

  SECTION("Errors") {
    CHECK_EXPR("1 ==", binaryExprNode(INT(1), EQEQ, errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "1 ====",
        binaryExprNode(INT(1), EQEQ, errInvalidUnaryOp(EQEQ, errInvalidPrimaryExpr(END))));
  }
}

} // namespace parse
