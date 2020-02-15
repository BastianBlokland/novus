#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace lex {

TEST_CASE("Lexing keywords", "[lex]") {
  CHECK_TOKENS("import", keywordToken(Keyword::Import));
  CHECK_TOKENS("fun", keywordToken(Keyword::Fun));
  CHECK_TOKENS("action", keywordToken(Keyword::Action));
  CHECK_TOKENS("self", keywordToken(Keyword::Self));
  CHECK_TOKENS("lambda", keywordToken(Keyword::Lambda));
  CHECK_TOKENS("fork", keywordToken(Keyword::Fork));
  CHECK_TOKENS("struct", keywordToken(Keyword::Struct));
  CHECK_TOKENS("union", keywordToken(Keyword::Union));
  CHECK_TOKENS("enum", keywordToken(Keyword::Enum));
  CHECK_TOKENS("if", keywordToken(Keyword::If));
  CHECK_TOKENS("else", keywordToken(Keyword::Else));
  CHECK_TOKENS("is", keywordToken(Keyword::Is));
  CHECK_TOKENS("as", keywordToken(Keyword::As));
}

} // namespace lex
