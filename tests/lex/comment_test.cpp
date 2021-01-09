#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"

namespace lex {

TEST_CASE("[lex] Lexing comments", "lex") {

  SECTION("Valid") {
    CHECK_TOKENS("//hello world", lineCommentToken("hello world"));
    CHECK_TOKENS("// hello world", lineCommentToken(" hello world"));
    CHECK_TOKENS("// hello \" world \" !@#", lineCommentToken(" hello \" world \" !@#"));
    CHECK_TOKENS("// hello\n 42 ", lineCommentToken(" hello"), litIntToken(42));
  }

  SECTION("Sequences") {
    CHECK_TOKENS("// hello\n// world ", lineCommentToken(" hello"), lineCommentToken(" world "));
    CHECK_TOKENS("// hello\n 42 ", lineCommentToken(" hello"), litIntToken(42));
  }

  SECTION("Spans") {
    CHECK_SPANS("// hello world ", input::Span{0, 14});
    CHECK_SPANS(" // hello\n// world ", input::Span{1, 9});
  }
}

} // namespace lex
