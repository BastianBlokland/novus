#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"

namespace lex {

TEST_CASE("[lex] Lexing string literals", "lex") {

  SECTION("Single values") {
    CHECK_TOKENS("\"hello\"", litStrToken("hello"));
    CHECK_TOKENS("\"hello world\"", litStrToken("hello world"));
    CHECK_TOKENS("\"你好，世界\"", litStrToken("你好，世界"));
  }

  SECTION("Escaping") {
    CHECK_TOKENS("\"hello\\nworld\"", litStrToken("hello\nworld"));
    CHECK_TOKENS("\"\\\"hello world\\\"\"", litStrToken("\"hello world\""));
    CHECK_TOKENS(
        "\"\\\" \\\\ \\a \\b \\f \\n \\r \\t \\v\"", litStrToken("\" \\ \a \b \f \n \r \t \v"));
  }

  SECTION("Sequences") {
    CHECK_TOKENS("\"hello\"\"world\"", litStrToken("hello"), litStrToken("world"));
    CHECK_TOKENS(
        "\"hello\",\"world\"",
        litStrToken("hello"),
        basicToken(TokenKind::SepComma),
        litStrToken("world"));
  }

  SECTION("Errors") {
    CHECK_TOKENS("\"", erLitStrUnterminated());
    CHECK_TOKENS("\"hello\n", erLitStrUnterminated());
    CHECK_TOKENS("\"hello\r", erLitStrUnterminated());
    CHECK_TOKENS("\"hello\\1world\"", errLitStrInvalidEscape());
    CHECK_TOKENS("\"hello\\1world", erLitStrUnterminated());
  }

  SECTION("Spans") {
    CHECK_SPANS(" \"Hello world\" ", input::Span{1, 13});
    CHECK_SPANS(" \"Hello\\1 world\" ", input::Span{1, 15});
    CHECK_SPANS(" \"Hello world ", input::Span{1, 13});
  }
}

} // namespace lex
