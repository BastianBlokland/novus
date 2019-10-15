#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"
#include <string>

namespace lex {

TEST_CASE("Lexing seperators", "[lex]") {
  CHECK_TOKENS("(", basicToken(TokenType::SepOpenParan));
  CHECK_TOKENS(")", basicToken(TokenType::SepCloseParan));
  CHECK_TOKENS(",", basicToken(TokenType::SepComma));
  CHECK_TOKENS("->", basicToken(TokenType::SepArrow));
}

} // namespace lex
