#include "catch2/catch.hpp"
#include "helpers.hpp"
#include <string>

namespace lex {

TEST_CASE("Lexing keywords", "[lex]") {
  CHECK_TOKENS("if", keywordToken(Keyword::If));
  CHECK_TOKENS("else", keywordToken(Keyword::Else));
  CHECK_TOKENS("print", keywordToken(Keyword::Print));
}

} // namespace lex
