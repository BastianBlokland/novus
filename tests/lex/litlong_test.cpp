#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"

namespace lex {

TEST_CASE("Lexing long literals", "[lex]") {

  SECTION("Single values") {
    CHECK_TOKENS("0l", litLongToken(0));
    CHECK_TOKENS("42L", litLongToken(42));
    CHECK_TOKENS("00042l", litLongToken(42));
    CHECK_TOKENS("9223372036854775807L", litLongToken(9223372036854775807L));
  }

  SECTION("Sequences") {
    CHECK_TOKENS("0l 0L", litLongToken(0), litLongToken(0));
    CHECK_TOKENS("1L 2l 3L", litLongToken(1), litLongToken(2), litLongToken(3));
    CHECK_TOKENS("1l,2L", litLongToken(1), basicToken(TokenKind::SepComma), litLongToken(2));
  }

  SECTION("Digit seperators") {
    CHECK_TOKENS("1_000L", litLongToken(1000));
    CHECK_TOKENS("1_2___3L", litLongToken(123));
  }

  SECTION("Errors") {
    CHECK_TOKENS("9223372036854775808L", errLitLongTooBig());
    CHECK_TOKENS("999999999999999999999L", errLitLongTooBig());
    CHECK_TOKENS("23234234349999999999L", errLitLongTooBig());
    CHECK_TOKENS("13a4a2L", errLitNumberInvalidChar());
    CHECK_TOKENS("13a4好2L", errLitNumberInvalidChar());
    CHECK_TOKENS("13a4\a2L", errLitNumberInvalidChar());
    CHECK_TOKENS("0aL", errLitNumberInvalidChar());
  }

  SECTION("Spans") {
    CHECK_SPANS(" 123L ", input::Span{1, 4});
    CHECK_SPANS(" 123L  0L", input::Span{1, 4}, input::Span{7, 8});
    CHECK_SPANS("1__2__3L", input::Span{0, 7});
    CHECK_SPANS("99999999999999999999999999L", input::Span{0, 26});
    CHECK_SPANS("12a12L", input::Span{0, 5});
    CHECK_SPANS("11____L", input::Span{0, 6});
  }
}

} // namespace lex
