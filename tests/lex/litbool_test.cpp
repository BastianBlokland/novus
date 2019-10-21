#include "catch2/catch.hpp"
#include "helpers.hpp"

namespace lex {

TEST_CASE("Lexing boolean literals", "[lex]") {

  SECTION("Single values") {
    CHECK_TOKENS("true", litBoolToken(true));
    CHECK_TOKENS("false", litBoolToken(false));
  }

  SECTION("Sequences") {
    CHECK_TOKENS("true false", litBoolToken(true), litBoolToken(false));
    CHECK_TOKENS(
        "true,true", litBoolToken(true), basicToken(TokenType::SepComma), litBoolToken(true));
  }

  SECTION("Spans") { CHECK_SPANS(" true ", SourceSpan{1, 4}); }
}

} // namespace lex
