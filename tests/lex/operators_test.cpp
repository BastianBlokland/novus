#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace lex {

TEST_CASE("Lexing operators", "[lex]") {
  CHECK_TOKENS("+", basicToken(TokenType::OpPlus));
  CHECK_TOKENS("-", basicToken(TokenType::OpMinus));
  CHECK_TOKENS("*", basicToken(TokenType::OpStar));
  CHECK_TOKENS("/", basicToken(TokenType::OpSlash));
  CHECK_TOKENS("&", basicToken(TokenType::OpAmp));
  CHECK_TOKENS("&&", basicToken(TokenType::OpAmpAmp));
  CHECK_TOKENS("|", basicToken(TokenType::OpPipe));
  CHECK_TOKENS("||", basicToken(TokenType::OpPipePipe));
  CHECK_TOKENS("=", basicToken(TokenType::OpEq));
  CHECK_TOKENS("==", basicToken(TokenType::OpEqEq));
  CHECK_TOKENS("!", basicToken(TokenType::OpBang));
  CHECK_TOKENS("!=", basicToken(TokenType::OpBangEq));
  CHECK_TOKENS("<", basicToken(TokenType::OpLess));
  CHECK_TOKENS("<=", basicToken(TokenType::OpLessEq));
  CHECK_TOKENS(">", basicToken(TokenType::OpGt));
  CHECK_TOKENS(">=", basicToken(TokenType::OpGtEq));
  CHECK_TOKENS(";", basicToken(TokenType::OpSemi));
}

} // namespace lex
