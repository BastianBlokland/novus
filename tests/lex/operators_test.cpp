#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"
#include <string>

namespace lex {

TEST_CASE("Lexing operators", "[lexer]") {
  REQUIRE_TOKENS("+", basicToken(TokenType::OpPlus));
  REQUIRE_TOKENS("-", basicToken(TokenType::OpMinus));
  REQUIRE_TOKENS("*", basicToken(TokenType::OpStar));
  REQUIRE_TOKENS("/", basicToken(TokenType::OpSlash));
  REQUIRE_TOKENS("&", basicToken(TokenType::OpAmp));
  REQUIRE_TOKENS("&&", basicToken(TokenType::OpAmpAmp));
  REQUIRE_TOKENS("|", basicToken(TokenType::OpPipe));
  REQUIRE_TOKENS("||", basicToken(TokenType::OpPipePipe));
  REQUIRE_TOKENS("=", basicToken(TokenType::OpEq));
  REQUIRE_TOKENS("==", basicToken(TokenType::OpEqEq));
  REQUIRE_TOKENS("!", basicToken(TokenType::OpBang));
  REQUIRE_TOKENS("!=", basicToken(TokenType::OpBangEq));
  REQUIRE_TOKENS("<", basicToken(TokenType::OpLess));
  REQUIRE_TOKENS("<=", basicToken(TokenType::OpLessEq));
  REQUIRE_TOKENS(">", basicToken(TokenType::OpGt));
  REQUIRE_TOKENS(">=", basicToken(TokenType::OpGtEq));
}

} // namespace lex
