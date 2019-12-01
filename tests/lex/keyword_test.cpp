#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace lex {

TEST_CASE("Lexing keywords", "[lex]") {
  CHECK_TOKENS("fun", keywordToken(Keyword::Fun));
  CHECK_TOKENS("struct", keywordToken(Keyword::Struct));
  CHECK_TOKENS("if", keywordToken(Keyword::If));
  CHECK_TOKENS("else", keywordToken(Keyword::Else));
}

} // namespace lex
