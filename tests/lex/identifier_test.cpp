#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"

namespace lex {

TEST_CASE("Lexing identifiers", "[lex]") {

  SECTION("Valid") {
    CHECK_TOKENS("hello", identiferToken("hello"));
    CHECK_TOKENS("héllo", identiferToken("héllo"));
    CHECK_TOKENS("_hello", identiferToken("_hello"));
    CHECK_TOKENS("_hello_world", identiferToken("_hello_world"));
    CHECK_TOKENS("_hello123", identiferToken("_hello123"));
    CHECK_TOKENS("_1", identiferToken("_1"));
    CHECK_TOKENS("_1", identiferToken("_1"));
    CHECK_TOKENS("你好世界", identiferToken("你好世界"));
    CHECK_TOKENS("_你好世界", identiferToken("_你好世界"));
    CHECK_TOKENS("_你1好2世3界", identiferToken("_你1好2世3界"));
    CHECK_TOKENS("_", basicToken(TokenKind::Discard));
  }

  SECTION("Sequences") {
    CHECK_TOKENS("hello world", identiferToken("hello"), identiferToken("world"));
    CHECK_TOKENS(
        "hello,world",
        identiferToken("hello"),
        basicToken(TokenKind::SepComma),
        identiferToken("world"));
    CHECK_TOKENS(
        "hello;world",
        identiferToken("hello"),
        basicToken(TokenKind::OpSemi),
        identiferToken("world"));
    CHECK_TOKENS(
        "hello@world", identiferToken("hello"), errInvalidChar('@'), identiferToken("world"));
    CHECK_TOKENS(
        "hello#world", identiferToken("hello"), errInvalidChar('#'), identiferToken("world"));
  }

  SECTION("Errors") {
    CHECK_TOKENS("1hello", errLitNumberInvalidChar());
    CHECK_TOKENS("h\a_hello", errIdentifierIllegalCharacter());
    CHECK_TOKENS("@", errInvalidChar('@'));

    // Ids containing '__' are reserved for internal use.
    CHECK_TOKENS("__", errIdentifierIllegalSequence());
    CHECK_TOKENS("___", errIdentifierIllegalSequence());
    CHECK_TOKENS("__hello", errIdentifierIllegalSequence());
    CHECK_TOKENS("hello__", errIdentifierIllegalSequence());
    CHECK_TOKENS("hello__world", errIdentifierIllegalSequence());
  }

  SECTION("Spans") {
    CHECK_SPANS(" helloworld ", input::Span{1, 10});
    CHECK_SPANS(" helloworld__ ", input::Span{1, 12});
    CHECK_SPANS(" __helloworld ", input::Span{1, 12});
    CHECK_SPANS(" _1 ", input::Span{1, 2});
  }
}

} // namespace lex
