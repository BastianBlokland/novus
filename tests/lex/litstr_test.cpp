#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"
#include <string>

namespace lex {

TEST_CASE("Lexing string literals", "[lex]") {

  SECTION("Single values") {
    REQUIRE_TOKENS("\"hello\"", litStrToken("hello"));
    REQUIRE_TOKENS("\"hello world\"", litStrToken("hello world"));
    REQUIRE_TOKENS("\"你好，世界\"", litStrToken("你好，世界"));
  }

  SECTION("Escaping") {
    REQUIRE_TOKENS("\"hello\\nworld\"", litStrToken("hello\nworld"));
    REQUIRE_TOKENS("\"\\\"hello world\\\"\"", litStrToken("\"hello world\""));
    REQUIRE_TOKENS(
        "\"\\\" \\\\ \\a \\b \\f \\n \\r \\t \\v\"", litStrToken("\" \\ \a \b \f \n \r \t \v"));
  }

  SECTION("Sequences") {
    REQUIRE_TOKENS("\"hello\"\"world\"", litStrToken("hello"), litStrToken("world"));
    REQUIRE_TOKENS(
        "\"hello\",\"world\"",
        litStrToken("hello"),
        basicToken(TokenType::SepComma),
        litStrToken("world"));
  }

  SECTION("Errors") {
    REQUIRE_TOKENS("\"", erLitStrUnterminated());
    REQUIRE_TOKENS("\"hello\n", erLitStrUnterminated());
    REQUIRE_TOKENS("\"hello\r", erLitStrUnterminated());
    REQUIRE_TOKENS("\"hello\\1world\"", errLitStrInvalidEscape());
    REQUIRE_TOKENS("\"hello\\1world", erLitStrUnterminated());
  }

  SECTION("Spans") {
    REQUIRE_SPANS(" \"Hello world\" ", SourceSpan{1, 13});
    REQUIRE_SPANS(" \"Hello\\1 world\" ", SourceSpan{1, 15});
    REQUIRE_SPANS(" \"Hello world ", SourceSpan{1, 13});
  }
}

} // namespace lex
