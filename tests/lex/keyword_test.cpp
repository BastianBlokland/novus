#include "catch2/catch.hpp"
#include "helpers.hpp"
#include <string>

namespace lex {

TEST_CASE("Lexing keywords", "[lexer]") {

  SECTION("Function") {
    REQUIRE_TOKENS("fun", keywordToken(Keyword::Function));
    REQUIRE_SPANS(" fun ", SourceSpan{1, 3})
  }
}

} // namespace lex
