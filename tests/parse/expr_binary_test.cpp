#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"

namespace parse {

TEST_CASE("Parsing binary operators", "[parse]") {

  CHECK_EXPR("-1 + 2", binaryExprNode(unaryExprNode(MINUS, INT(1)), PLUS, INT(2)));
  CHECK_EXPR("y == false", binaryExprNode(ID_EXPR("y"), EQEQ, BOOL(false)));
  CHECK_EXPR("1 + 2 + 3", binaryExprNode(binaryExprNode(INT(1), PLUS, INT(2)), PLUS, INT(3)));
  CHECK_EXPR(
      "1 + 3 1 * 2", binaryExprNode(INT(1), PLUS, INT(3)), binaryExprNode(INT(1), STAR, INT(2)));
  CHECK_EXPR(
      "1 ! 3 1 * 2", binaryExprNode(INT(1), BANG, INT(3)), binaryExprNode(INT(1), STAR, INT(2)));
  CHECK_EXPR("1 ! !3", binaryExprNode(INT(1), BANG, unaryExprNode(BANG, INT(3))));
  CHECK_EXPR(
      "!1 ! !3", binaryExprNode(unaryExprNode(BANG, INT(1)), BANG, unaryExprNode(BANG, INT(3))));

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
            ID_EXPR("x"),
            PIPEPIPE,
            binaryExprNode(
                ID_EXPR("y"),
                AMPAMP,
                binaryExprNode(
                    unaryExprNode(MINUS, ID_EXPR("z")),
                    EQEQ,
                    binaryExprNode(
                        ID_EXPR("q"),
                        GT,
                        binaryExprNode(
                            unaryExprNode(MINUS, INT(5)),
                            MINUS,
                            binaryExprNode(INT(2), STAR, INT(3))))))));
    CHECK_EXPR(
        "1 :: 2 :: 3",
        binaryExprNode(INT(1), COLONCOLON, binaryExprNode(INT(2), COLONCOLON, INT(3))));
    CHECK_EXPR(
        "a{int} + b{float}",
        binaryExprNode(
            ID_EXPR_PARAM("a", TypeParamList(OCURLY, {TYPE("int")}, COMMAS(0), CCURLY)),
            PLUS,
            ID_EXPR_PARAM("b", TypeParamList(OCURLY, {TYPE("float")}, COMMAS(0), CCURLY))));
  }

  SECTION("Errors") {
    CHECK_EXPR("1 ==", binaryExprNode(INT(1), EQEQ, errInvalidPrimaryExpr(END)));
    CHECK_EXPR(
        "1 ====",
        binaryExprNode(INT(1), EQEQ, errInvalidUnaryOp(EQEQ, errInvalidPrimaryExpr(END))));
  }

  SECTION("Spans") { CHECK_EXPR_SPAN("1 + 2 == 2 + 3", input::Span(0, 13)); }
}

} // namespace parse
