#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"
#include <string>

namespace lex {

TEST_CASE("Lexing seperators", "[lexer]") {
  REQUIRE_TOKENS("(", basicToken(TokenType::SepOpenParan));
  REQUIRE_TOKENS(")", basicToken(TokenType::SepCloseParan));
  REQUIRE_TOKENS(",", basicToken(TokenType::SepComma));
  REQUIRE_TOKENS("->", basicToken(TokenType::SepArrow));
}

} // namespace lex
