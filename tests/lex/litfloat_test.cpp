#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"

namespace lex {

TEST_CASE("Lexing float literals", "[lex]") {

  SECTION("Single values") {
    CHECK_TOKENS("0.0", litFloatToken(.0F));
    CHECK_TOKENS(".0", litFloatToken(.0F));
    CHECK_TOKENS("00042.0", litFloatToken(42.0F));                        // NOLINT: Magic numbers
    CHECK_TOKENS("42.000000", litFloatToken(42.0F));                      // NOLINT: Magic numbers
    CHECK_TOKENS(".999999", litFloatToken(.999999F));                     // NOLINT: Magic numbers
    CHECK_TOKENS("999999.0", litFloatToken(999999.0F));                   // NOLINT: Magic numbers
    CHECK_TOKENS("999.9999", litFloatToken(999.9999F));                   // NOLINT: Magic numbers
    CHECK_TOKENS("9999.9999", litFloatToken(9999.9999F));                 // NOLINT: Magic numbers
    CHECK_TOKENS("184467440737095.0", litFloatToken(184467440737095.0F)); // NOLINT: Magic numbers
    CHECK_TOKENS(
        ".000000000000001337", litFloatToken(.000000000000001337F)); // NOLINT: Magic numbers
  }

  SECTION("Sequences") {
    CHECK_TOKENS("0.0 .0", litFloatToken(.0F), litFloatToken(.0F));
    CHECK_TOKENS(
        "1.0 0.2 .3",
        litFloatToken(1.0F),
        litFloatToken(.2F),  // NOLINT: Magic numbers
        litFloatToken(.3F)); // NOLINT: Magic numbers
    CHECK_TOKENS(
        "1.42,2.1337",
        litFloatToken(1.42F), // NOLINT: Magic numbers
        basicToken(TokenKind::SepComma),
        litFloatToken(2.1337F)); // NOLINT: Magic numbers
    CHECK_TOKENS("..0", basicToken(TokenKind::OpDot), litFloatToken(.0F));
  }

  SECTION("Digit seperators") {
    CHECK_TOKENS("1_000.0", litFloatToken(1000.0F));         // NOLINT: Magic numbers
    CHECK_TOKENS("1_2___3.0", litFloatToken(123.0F));        // NOLINT: Magic numbers
    CHECK_TOKENS("1_2___3.__0_01", litFloatToken(123.001F)); // NOLINT: Magic numbers
  }

  SECTION("Errors") {
    CHECK_TOKENS("18446744073709551615.0", errLitFloatUnrepresentable());
    CHECK_TOKENS("123.00000000000000000001", errLitFloatUnrepresentable());
    CHECK_TOKENS("13a4a2.0", errLitNumberInvalidChar());
    CHECK_TOKENS("13a4好2.0", errLitNumberInvalidChar());
    CHECK_TOKENS("13a4\a2.0", errLitNumberInvalidChar());
    CHECK_TOKENS("0a.0", errLitNumberInvalidChar());
    CHECK_TOKENS(".0_", errLitNumberEndsWithSeperator());
    CHECK_TOKENS(".0.0", errLitNumberInvalidChar());
  }

  SECTION("Spans") {
    CHECK_SPANS(" 123.0 ", input::Span{1, 5});
    CHECK_SPANS(" 123.0  .0", input::Span{1, 5}, input::Span{8, 9});
    CHECK_SPANS("1__2__3.1__2__3", input::Span{0, 14});
    CHECK_SPANS("9999_9999._9999_9999_9999", input::Span{0, 24});
    CHECK_SPANS("12.a12", input::Span{0, 5});
    CHECK_SPANS("11.____", input::Span{0, 6});
  }
}

} // namespace lex
