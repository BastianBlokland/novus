#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"

namespace lex {

TEST_CASE("Lexing integer literals", "[lexer]") {

  SECTION("Single values") {
    REQUIRE_TOKENS("0", litIntToken(0));
    REQUIRE_TOKENS("42", litIntToken(42));
    REQUIRE_TOKENS("00042", litIntToken(42));
    REQUIRE_TOKENS("2147483647", litIntToken(2147483647));
  }

  SECTION("Sequences") {
    REQUIRE_TOKENS("0 0", litIntToken(0), litIntToken(0));
    REQUIRE_TOKENS("1 2 3", litIntToken(1), litIntToken(2), litIntToken(3));
    REQUIRE_TOKENS("1,2", litIntToken(1), basicToken(TokenType::SepComma), litIntToken(2));
    REQUIRE_TOKENS("1;2", litIntToken(1), errInvalidChar(';'), litIntToken(2));
  }

  SECTION("Digit seperators") {
    REQUIRE_TOKENS("1_000", litIntToken(1000));
    REQUIRE_TOKENS("1_2___3", litIntToken(123));
  }

  SECTION("Errors") {
    REQUIRE_TOKENS("2147483648", errLitIntTooBig());
    REQUIRE_TOKENS("99999999999999999999", errLitIntTooBig());
    REQUIRE_TOKENS("13a4a2", errLitIntInvalidChar());
    REQUIRE_TOKENS("13a4好2", errLitIntInvalidChar());
    REQUIRE_TOKENS("13a4\a2", errLitIntInvalidChar());
    REQUIRE_TOKENS("0a", errLitIntInvalidChar());
    REQUIRE_TOKENS("0_", errLitIntEndsWithSeperator());
  }

  SECTION("Spans") {
    REQUIRE_SPANS(" 123 ", SourceSpan{1, 3});
    REQUIRE_SPANS(" 123  0", SourceSpan{1, 3}, SourceSpan{6});
    REQUIRE_SPANS("1__2__3", SourceSpan{0, 6});
    REQUIRE_SPANS("99999999999999999999", SourceSpan{0, 19});
    REQUIRE_SPANS("12a12", SourceSpan{0, 4});
    REQUIRE_SPANS("11____", SourceSpan{0, 5});
  }
}

} // namespace lex
