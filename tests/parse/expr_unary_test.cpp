#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "parse/error.hpp"

namespace parse {

TEST_CASE("Parsing unary operators", "[parse]") {

  CHECK_EXPR("-1", unaryExprNode(MINUS, INT(1)));
  CHECK_EXPR("+1", unaryExprNode(PLUS, INT(1)));
  CHECK_EXPR("!1", unaryExprNode(BANG, INT(1)));
  CHECK_EXPR("!x", unaryExprNode(BANG, ID_EXPR("x")));
  CHECK_EXPR("?1", unaryExprNode(QMARK, INT(1)));
  CHECK_EXPR("--1", unaryExprNode(MINUSMINUS, INT(1)));
  CHECK_EXPR("++1", unaryExprNode(PLUSPLUS, INT(1)));
  CHECK_EXPR("-+-1", unaryExprNode(MINUS, unaryExprNode(PLUS, unaryExprNode(MINUS, INT(1)))));
  CHECK_EXPR("+-!1", unaryExprNode(PLUS, unaryExprNode(MINUS, unaryExprNode(BANG, INT(1)))));
  CHECK_EXPR("+?!1", unaryExprNode(PLUS, unaryExprNode(QMARK, unaryExprNode(BANG, INT(1)))));
  CHECK_EXPR("!42 !true", unaryExprNode(BANG, INT(42)), unaryExprNode(BANG, BOOL(true)));

  SECTION("Errors") {
    CHECK_EXPR("&1", errInvalidUnaryOp(AMP, INT(1)));
    CHECK_EXPR("??1", errInvalidUnaryOp(QMARKQMARK, INT(1)));
    CHECK_EXPR("&+1", errInvalidUnaryOp(AMP, unaryExprNode(PLUS, INT(1))));
    CHECK_EXPR("&1 + 2", errInvalidUnaryOp(AMP, binaryExprNode(INT(1), PLUS, INT(2))));
    CHECK_EXPR("&", errInvalidUnaryOp(AMP, errInvalidPrimaryExpr(END)));
  }

  SECTION("Spans") {
    CHECK_EXPR_SPAN("-1", input::Span(0, 1));
    CHECK_EXPR_SPAN(" +-- ! 1 + 2 ", input::Span(1, 11));
  }
}

} // namespace parse
