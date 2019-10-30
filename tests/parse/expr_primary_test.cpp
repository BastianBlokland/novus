#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"
#include "parse/error.hpp"

namespace parse {

TEST_CASE("Parsing primary expressions", "[parse]") {

  CHECK_EXPR("1 true \"hello world\" 42", INT(1), BOOL(true), STR("hello world"), INT(42));
  CHECK_EXPR("x y z", CONST("x"), CONST("y"), CONST("z"));

  SECTION("Errors") {
    CHECK_EXPR("12a", errLexError(lex::errLitIntInvalidChar()))
    CHECK_EXPR("->", errInvalidPrimaryExpr(lex::basicToken(lex::TokenKind::SepArrow)));
    CHECK_EXPR("fun", errInvalidPrimaryExpr(lex::keywordToken(lex::Keyword::Fun)));
  }

  SECTION("Const declaration") {
    CHECK_EXPR("x = 1", CONSTDECL("x", INT(1)));
    CHECK_EXPR(
        "x = 1; y = 2",
        groupExprNode(NODES(CONSTDECL("x", INT(1)), CONSTDECL("y", INT(2))), SEMIS(1)));
    CHECK_EXPR(
        "x = 1 + 2; y = 2",
        groupExprNode(
            NODES(CONSTDECL("x", binaryExprNode(INT(1), PLUS, INT(2))), CONSTDECL("y", INT(2))),
            SEMIS(1)));

    SECTION("Errors") { CHECK_EXPR("x =", CONSTDECL("x", errInvalidPrimaryExpr(END))); }
  }

  SECTION("Spans") {
    CHECK_EXPR_SPAN("123", input::SourceSpan(0, 2));
    CHECK_EXPR_SPAN("x = 123", input::SourceSpan(0, 6));
  }
}

} // namespace parse
