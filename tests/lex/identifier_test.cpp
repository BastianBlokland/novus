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
  }

  SECTION("Sequences") {
    CHECK_TOKENS("hello world", identiferToken("hello"), identiferToken("world"));
    CHECK_TOKENS(
        "hello,world",
        identiferToken("hello"),
        basicToken(TokenType::SepComma),
        identiferToken("world"));
    CHECK_TOKENS(
        "hello;world",
        identiferToken("hello"),
        basicToken(TokenType::OpSemi),
        identiferToken("world"));
    CHECK_TOKENS(
        "hello@world", identiferToken("hello"), errInvalidChar('@'), identiferToken("world"));
    CHECK_TOKENS(
        "hello#world", identiferToken("hello"), errInvalidChar('#'), identiferToken("world"));
  }

  SECTION("Errors") {
    CHECK_TOKENS("1hello", errLitIntInvalidChar());
    CHECK_TOKENS("h\a_hello", errIdentifierIllegalCharacter());
    CHECK_TOKENS("@", errInvalidChar('@'));
    CHECK_TOKENS("_", errInvalidChar('_'));

    // Ids containing '__' are reserved for internal use.
    CHECK_TOKENS("__", errIdentifierIllegalSequence());
    CHECK_TOKENS("___", errIdentifierIllegalSequence());
    CHECK_TOKENS("__hello", errIdentifierIllegalSequence());
    CHECK_TOKENS("hello__", errIdentifierIllegalSequence());
    CHECK_TOKENS("hello__world", errIdentifierIllegalSequence());
  }

  SECTION("Spans") {
    CHECK_SPANS(" helloworld ", SourceSpan{1, 10});
    CHECK_SPANS(" helloworld__ ", SourceSpan{1, 12});
    CHECK_SPANS(" __helloworld ", SourceSpan{1, 12});
    CHECK_SPANS(" _1 ", SourceSpan{1, 2});
  }
}

} // namespace lex
