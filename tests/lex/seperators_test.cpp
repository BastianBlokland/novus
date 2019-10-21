#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace lex {

TEST_CASE("Lexing seperators", "[lex]") {
  CHECK_TOKENS("(", basicToken(TokenType::SepOpenParen));
  CHECK_TOKENS(")", basicToken(TokenType::SepCloseParen));
  CHECK_TOKENS(",", basicToken(TokenType::SepComma));
  CHECK_TOKENS("->", basicToken(TokenType::SepArrow));
}

} // namespace lex
