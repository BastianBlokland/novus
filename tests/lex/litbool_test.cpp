#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/lexer.hpp"

namespace lex {

TEST_CASE("Lexing boolean literals", "[lexer]") {

  SECTION("Single values") {
    REQUIRE_TOKENS("true", litBoolToken(true));
    REQUIRE_TOKENS("false", litBoolToken(false));
  }

  SECTION("Sequences") {
    REQUIRE_TOKENS("true false", litBoolToken(true), litBoolToken(false));
    REQUIRE_TOKENS("true,true", litBoolToken(true), basicToken(TokenType::SepComma),
                   litBoolToken(true));
  }

  SECTION("Spans") { REQUIRE_SPANS(" true ", SourceSpan{1, 4}); }
}

} // namespace lex
