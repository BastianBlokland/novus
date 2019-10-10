#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"
#include <string>

namespace lex {

TEST_CASE("Lexing identifiers", "[lexer]") {

  SECTION("Valid") {
    REQUIRE_TOKENS("hello", identiferToken("hello"));
    REQUIRE_TOKENS("héllo", identiferToken("héllo"));
    REQUIRE_TOKENS("_hello", identiferToken("_hello"));
    REQUIRE_TOKENS("_hello_world", identiferToken("_hello_world"));
    REQUIRE_TOKENS("_hello123", identiferToken("_hello123"));
    REQUIRE_TOKENS("_1", identiferToken("_1"));
    REQUIRE_TOKENS("_1", identiferToken("_1"));
    REQUIRE_TOKENS("你好世界", identiferToken("你好世界"));
    REQUIRE_TOKENS("_你好世界", identiferToken("_你好世界"));
    REQUIRE_TOKENS("_你1好2世3界", identiferToken("_你1好2世3界"));
  }

  SECTION("Sequences") {
    REQUIRE_TOKENS("hello world", identiferToken("hello"), identiferToken("world"));
    REQUIRE_TOKENS(
        "hello,world", identiferToken("hello"), basicToken(TokenType::SepComma),
        identiferToken("world"));
    REQUIRE_TOKENS(
        "hello@world", identiferToken("hello"), errInvalidChar('@'), identiferToken("world"));
    REQUIRE_TOKENS(
        "hello#world", identiferToken("hello"), errInvalidChar('#'), identiferToken("world"));
  }

  SECTION("Errors") {
    REQUIRE_TOKENS("1hello", errLitIntInvalidChar());
    REQUIRE_TOKENS("h\a_hello", errIdentifierIllegalCharacter());
    REQUIRE_TOKENS("@", errInvalidChar('@'));
    REQUIRE_TOKENS("_", errInvalidChar('_'));

    // Ids containing '__' are reserved for internal use.
    REQUIRE_TOKENS("__", errIdentifierIllegalSequence());
    REQUIRE_TOKENS("___", errIdentifierIllegalSequence());
    REQUIRE_TOKENS("__hello", errIdentifierIllegalSequence());
    REQUIRE_TOKENS("hello__", errIdentifierIllegalSequence());
    REQUIRE_TOKENS("hello__world", errIdentifierIllegalSequence());
  }

  SECTION("Spans") {
    REQUIRE_SPANS(" helloworld ", SourceSpan{1, 10});
    REQUIRE_SPANS(" helloworld__ ", SourceSpan{1, 12});
    REQUIRE_SPANS(" __helloworld ", SourceSpan{1, 12});
    REQUIRE_SPANS(" _1 ", SourceSpan{1, 2});
  }
}

} // namespace lex
