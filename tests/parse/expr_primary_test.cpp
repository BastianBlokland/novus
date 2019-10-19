#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace parse {

TEST_CASE("Parsing primary expressions", "[parse]") {

  CHECK_EXPR("1 true \"hello world\" 42", INT(1), BOOL(true), STR("hello world"), INT(42));
  CHECK_EXPR("x y z", CONST("x"), CONST("y"), CONST("z"));

  SECTION("Errors") {
    CHECK_EXPR("12a", errLexError(lex::errLitIntInvalidChar()))
    CHECK_EXPR("->", errInvalidPrimaryExpr(lex::basicToken(lex::TokenType::SepArrow)));
    CHECK_EXPR("print", errInvalidPrimaryExpr(lex::keywordToken(lex::Keyword::Print)));
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
}

} // namespace parse
