#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"
#include "lex/lexer.hpp"
#include <string>

namespace lex {

TEST_CASE("Lexing integer literals", "[lexer]") {

  SECTION("Single values") {
    REQUIRE_TOKENS("0", litIntToken(0));
    REQUIRE_TOKENS("42", litIntToken(42));
    REQUIRE_TOKENS("2147483647", litIntToken(2147483647));
  }

  SECTION("Sequences") {
    REQUIRE_TOKENS("0 0", litIntToken(0), litIntToken(0));
    REQUIRE_TOKENS("1 2 3", litIntToken(1), litIntToken(2), litIntToken(3));
    REQUIRE_TOKENS("1,2", litIntToken(1), basicToken(TokenType::SepComma), litIntToken(2));
  }

  SECTION("Errors") {
    REQUIRE_TOKENS("2147483648", errLitIntTooBig());
    REQUIRE_TOKENS("99999999999999999999", errLitIntTooBig());
    REQUIRE_TOKENS("13a4a2", errLitIntInvalidChar());
    REQUIRE_TOKENS("0a", errLitIntInvalidChar());
  }

  SECTION("Spans") {
    REQUIRE_SPANS(" 123 ", SourceSpan{1, 3});
    REQUIRE_SPANS(" 123  0", SourceSpan{1, 3}, SourceSpan{6});
  }
}

} // namespace lex
