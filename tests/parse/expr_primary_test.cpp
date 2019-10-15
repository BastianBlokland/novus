#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"
#include "parse/error.hpp"
#include "parse/node_expr_binary.hpp"
#include "parse/node_expr_paren.hpp"
#include "parse/node_expr_unary.hpp"

namespace parse {

TEST_CASE("Parsing primary expressions", "[parse]") {

  CHECK_EXPR("1 true \"hello world\" 42", INT(1), BOOL(true), STR("hello world"), INT(42));
  CHECK_EXPR("x y z", CONST("x"), CONST("y"), CONST("z"));

  SECTION("Errors") {
    CHECK_EXPR("12a", errLexError(lex::errLitIntInvalidChar()))
    CHECK_EXPR("->", errInvalidPrimaryExpr(lex::basicToken(lex::TokenType::SepArrow)));
    CHECK_EXPR("print", errInvalidPrimaryExpr(lex::keywordToken(lex::Keyword::Print)));
  }
}

} // namespace parse
