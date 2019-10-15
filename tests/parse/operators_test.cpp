#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"
#include "lex/keyword.hpp"
#include "lex/token.hpp"
#include "lex/token_type.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_paren.hpp"
#include "parse/node_expr_unary.hpp"

namespace parse {

TEST_CASE("Parsing operators", "[parse]") {

  SECTION("Primary") {
    CHECK_EXPR("1 true \"hello world\" 42", INT(1), BOOL(true), STR("hello world"), INT(42));
    CHECK_EXPR("x y z", CONST("x"), CONST("y"), CONST("z"));

    SECTION("Errors") {
      CHECK_EXPR("12a", errLexError(lex::errLitIntInvalidChar()))
      CHECK_EXPR("->", errInvalidPrimaryExpr(lex::basicToken(lex::TokenType::SepArrow)));
      CHECK_EXPR("print", errInvalidPrimaryExpr(lex::keywordToken(lex::Keyword::Print)));
    }
  }

  SECTION("Unary") {
    CHECK_EXPR("-1", unaryExprNode(MINUS, INT(1)))
    CHECK_EXPR("+1", unaryExprNode(PLUS, INT(1)))
    CHECK_EXPR("!1", unaryExprNode(BANG, INT(1)))
    CHECK_EXPR("!x", unaryExprNode(BANG, CONST("x")))
    CHECK_EXPR("--1", unaryExprNode(MINUS, unaryExprNode(MINUS, INT(1))));
    CHECK_EXPR("++1", unaryExprNode(PLUS, unaryExprNode(PLUS, INT(1))));
    CHECK_EXPR("-+-1", unaryExprNode(MINUS, unaryExprNode(PLUS, unaryExprNode(MINUS, INT(1)))));
    CHECK_EXPR("+-!1", unaryExprNode(PLUS, unaryExprNode(MINUS, unaryExprNode(BANG, INT(1)))));
    CHECK_EXPR("!42 !true", unaryExprNode(BANG, INT(42)), unaryExprNode(BANG, BOOL(true)));

    SECTION("Errors") {
      CHECK_EXPR("&1", errInvalidUnaryOp(AMP, INT(1)));
      CHECK_EXPR("&+1", errInvalidUnaryOp(AMP, unaryExprNode(PLUS, INT(1))));
      CHECK_EXPR("&1 + 2", errInvalidUnaryOp(AMP, binaryExprNode(INT(1), PLUS, INT(2))));
      CHECK_EXPR("&", errInvalidUnaryOp(AMP, errInvalidPrimaryExpr(END)));
    }
  }

  SECTION("Binary") {
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

  SECTION("Parenthesized") {
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
  }
}

} // namespace parse
