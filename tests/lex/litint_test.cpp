#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"

namespace lex {

TEST_CASE("Lexing integer literals", "[lex]") {

  SECTION("Single values") {
    CHECK_TOKENS("0", litIntToken(0));
    CHECK_TOKENS("42", litIntToken(42));
    CHECK_TOKENS("00042", litIntToken(42));
    CHECK_TOKENS("2147483647", litIntToken(2147483647));
  }

  SECTION("Sequences") {
    CHECK_TOKENS("0 0", litIntToken(0), litIntToken(0));
    CHECK_TOKENS("1 2 3", litIntToken(1), litIntToken(2), litIntToken(3));
    CHECK_TOKENS("1,2", litIntToken(1), basicToken(TokenType::SepComma), litIntToken(2));
  }

  SECTION("Digit seperators") {
    CHECK_TOKENS("1_000", litIntToken(1000));
    CHECK_TOKENS("1_2___3", litIntToken(123));
  }

  SECTION("Errors") {
    CHECK_TOKENS("2147483648", errLitIntTooBig());
    CHECK_TOKENS("99999999999999999999", errLitIntTooBig());
    CHECK_TOKENS("13a4a2", errLitIntInvalidChar());
    CHECK_TOKENS("13a4好2", errLitIntInvalidChar());
    CHECK_TOKENS("13a4\a2", errLitIntInvalidChar());
    CHECK_TOKENS("0a", errLitIntInvalidChar());
    CHECK_TOKENS("0_", errLitIntEndsWithSeperator());
  }

  SECTION("Spans") {
    CHECK_SPANS(" 123 ", SourceSpan{1, 3});
    CHECK_SPANS(" 123  0", SourceSpan{1, 3}, SourceSpan{6});
    CHECK_SPANS("1__2__3", SourceSpan{0, 6});
    CHECK_SPANS("99999999999999999999", SourceSpan{0, 19});
    CHECK_SPANS("12a12", SourceSpan{0, 4});
    CHECK_SPANS("11____", SourceSpan{0, 5});
  }
}

} // namespace lex
