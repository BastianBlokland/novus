#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace lex {

TEST_CASE("[lex] Lexing seperators", "lex") {
  CHECK_TOKENS("(", basicToken(TokenKind::SepOpenParen));
  CHECK_TOKENS(")", basicToken(TokenKind::SepCloseParen));
  CHECK_TOKENS("{", basicToken(TokenKind::SepOpenCurly));
  CHECK_TOKENS("}", basicToken(TokenKind::SepCloseCurly));
  CHECK_TOKENS("[", basicToken(TokenKind::SepOpenSquare));
  CHECK_TOKENS("]", basicToken(TokenKind::SepCloseSquare));
  CHECK_TOKENS(",", basicToken(TokenKind::SepComma));
  CHECK_TOKENS(":", basicToken(TokenKind::SepColon));
  CHECK_TOKENS("->", basicToken(TokenKind::SepArrow));
}

} // namespace lex
