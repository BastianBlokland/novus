#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace lex {

TEST_CASE("[lex] Lexing operators", "lex") {
  CHECK_TOKENS("+", basicToken(TokenKind::OpPlus));
  CHECK_TOKENS("++", basicToken(TokenKind::OpPlusPlus));
  CHECK_TOKENS("-", basicToken(TokenKind::OpMinus));
  CHECK_TOKENS("--", basicToken(TokenKind::OpMinusMinus));
  CHECK_TOKENS("*", basicToken(TokenKind::OpStar));
  CHECK_TOKENS("/", basicToken(TokenKind::OpSlash));
  CHECK_TOKENS("%", basicToken(TokenKind::OpRem));
  CHECK_TOKENS("&", basicToken(TokenKind::OpAmp));
  CHECK_TOKENS("&&", basicToken(TokenKind::OpAmpAmp));
  CHECK_TOKENS("|", basicToken(TokenKind::OpPipe));
  CHECK_TOKENS("||", basicToken(TokenKind::OpPipePipe));
  CHECK_TOKENS("^", basicToken(TokenKind::OpHat));
  CHECK_TOKENS("~", basicToken(TokenKind::OpTilde));
  CHECK_TOKENS("<<", basicToken(TokenKind::OpShiftL));
  CHECK_TOKENS(">>", basicToken(TokenKind::OpShiftR));
  CHECK_TOKENS("=", basicToken(TokenKind::OpEq));
  CHECK_TOKENS("==", basicToken(TokenKind::OpEqEq));
  CHECK_TOKENS("!", basicToken(TokenKind::OpBang));
  CHECK_TOKENS("!!", basicToken(TokenKind::OpBangBang));
  CHECK_TOKENS("!=", basicToken(TokenKind::OpBangEq));
  CHECK_TOKENS("<", basicToken(TokenKind::OpLe));
  CHECK_TOKENS("<=", basicToken(TokenKind::OpLeEq));
  CHECK_TOKENS(">", basicToken(TokenKind::OpGt));
  CHECK_TOKENS(">=", basicToken(TokenKind::OpGtEq));
  CHECK_TOKENS(";", basicToken(TokenKind::OpSemi));
  CHECK_TOKENS("?", basicToken(TokenKind::OpQMark));
  CHECK_TOKENS("??", basicToken(TokenKind::OpQMarkQMark));
  CHECK_TOKENS(".", basicToken(TokenKind::OpDot));
  CHECK_TOKENS("::", basicToken(TokenKind::OpColonColon));
  CHECK_TOKENS("[]", basicToken(TokenKind::OpSquareSquare));
  CHECK_TOKENS("()", basicToken(TokenKind::OpParenParen));
}

} // namespace lex
