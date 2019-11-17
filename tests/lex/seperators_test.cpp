#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace lex {

TEST_CASE("Lexing seperators", "[lex]") {
  CHECK_TOKENS("(", basicToken(TokenKind::SepOpenParen));
  CHECK_TOKENS(")", basicToken(TokenKind::SepCloseParen));
  CHECK_TOKENS(",", basicToken(TokenKind::SepComma));
  CHECK_TOKENS(":", basicToken(TokenKind::SepColon));
  CHECK_TOKENS("->", basicToken(TokenKind::SepArrow));
}

} // namespace lex
