#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"

namespace lex {

TEST_CASE("[lex] Lexing long literals", "lex") {

  SECTION("Single values") {
    CHECK_TOKENS("0l", litLongToken(0));
    CHECK_TOKENS("42L", litLongToken(42));
    CHECK_TOKENS("00042l", litLongToken(42));
    CHECK_TOKENS("9223372036854775807L", litLongToken(9223372036854775807L));
  }

  SECTION("Hex values") {
    CHECK_TOKENS("0xL", litLongToken(0));
    CHECK_TOKENS("0XL", litLongToken(0));
    CHECK_TOKENS("0x0L", litLongToken(0));
    CHECK_TOKENS("0X0L", litLongToken(0));
    CHECK_TOKENS("0x4L", litLongToken(0x4));
    CHECK_TOKENS("0xFL", litLongToken(0xF));
    CHECK_TOKENS("0xFFL", litLongToken(0xFF));
    CHECK_TOKENS("0xfL", litLongToken(0xf));
    CHECK_TOKENS("0xFfL", litLongToken(0xFf));
    CHECK_TOKENS("0x7FffFfFfL", litLongToken(0x7FffFfFf));
    CHECK_TOKENS("0x2AL", litLongToken(42));
    CHECK_TOKENS("0x539L", litLongToken(1337));
    CHECK_TOKENS("0x1E240L", litLongToken(0x1E240));
    CHECK_TOKENS("0xFFFF_FFFF_FFFF_FFFFL", litLongToken(18446744073709551615UL));
  }

  SECTION("Binary values") {
    CHECK_TOKENS("0bL", litLongToken(0));
    CHECK_TOKENS("0Bl", litLongToken(0));
    CHECK_TOKENS("0b0L", litLongToken(0));
    CHECK_TOKENS("0B0L", litLongToken(0));
    CHECK_TOKENS("0b0001L", litLongToken(1));
    CHECK_TOKENS("0b10L", litLongToken(2));
    CHECK_TOKENS("0b11l", litLongToken(3));
    CHECK_TOKENS("0b101010L", litLongToken(42));
    CHECK_TOKENS("0b10100111001L", litLongToken(1337));
    CHECK_TOKENS("0b10010101_01010101L", litLongToken(0b1001010101010101));
    CHECK_TOKENS("0b01111111_11111111_11111111_11111111L", litLongToken(0x7FFFFFFF));
    CHECK_TOKENS("0b11111111_11111111_11111111_11111111L", litLongToken(4294967295));
    CHECK_TOKENS("0b01111111_11111111_11111111_11111111L", litLongToken(0x7FFFFFFF));
    CHECK_TOKENS(
        "0b11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111L",
        litLongToken(-1));
    CHECK_TOKENS(
        "0b01111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111L",
        litLongToken(9223372036854775807));
  }

  SECTION("Sequences") {
    CHECK_TOKENS("0l 0L", litLongToken(0), litLongToken(0));
    CHECK_TOKENS("1L 2l 3L", litLongToken(1), litLongToken(2), litLongToken(3));
    CHECK_TOKENS("1l,2L", litLongToken(1), basicToken(TokenKind::SepComma), litLongToken(2));

    CHECK_TOKENS("0xL 0x123l", litLongToken(0), litLongToken(0x123));
    CHECK_TOKENS("0x1L 0x2L 0x3l", litLongToken(1), litLongToken(2), litLongToken(3));
    CHECK_TOKENS("0xFl,0XL", litLongToken(15), basicToken(TokenKind::SepComma), litLongToken(0));

    CHECK_TOKENS("0bL 0b111l", litLongToken(0), litLongToken(0b111));
    CHECK_TOKENS("0b1L 0b01l 0b11L", litLongToken(0b1), litLongToken(0b01), litLongToken(0b11));
    CHECK_TOKENS("0b1l,0B1L", litLongToken(1), basicToken(TokenKind::SepComma), litLongToken(1));
  }

  SECTION("Digit seperators") {
    CHECK_TOKENS("1_000L", litLongToken(1000));
    CHECK_TOKENS("1_2___3L", litLongToken(123));

    CHECK_TOKENS("0xF_F_FL", litLongToken(0xFFF));
    CHECK_TOKENS("0xA_B___CL", litLongToken(0xABC));

    CHECK_TOKENS("0b1_1_1L", litLongToken(7));
    CHECK_TOKENS("0b1_0___1L", litLongToken(5));
  }

  SECTION("Errors") {
    CHECK_TOKENS("9223372036854775808L", errLitLongTooBig());
    CHECK_TOKENS("999999999999999999999L", errLitLongTooBig());
    CHECK_TOKENS("23234234349999999999L", errLitLongTooBig());
    CHECK_TOKENS("13a4a2L", errLitNumberInvalidChar());
    CHECK_TOKENS("13a4好2L", errLitNumberInvalidChar());
    CHECK_TOKENS("13a4\a2L", errLitNumberInvalidChar());
    CHECK_TOKENS("0aL", errLitNumberInvalidChar());

    CHECK_TOKENS("0xFFFF_FFFF_FFFF_FFFF_1L", errLitLongTooBig());
    CHECK_TOKENS("0x13ga4a2L", errLitHexInvalidChar());
    CHECK_TOKENS("0x13a4好2L", errLitHexInvalidChar());
    CHECK_TOKENS("0x13a4\a2L", errLitHexInvalidChar());

    CHECK_TOKENS(
        "0b11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_0L",
        errLitLongTooBig());
    CHECK_TOKENS(
        "0b11111111_11111111_11111111_11111111_11111111_11111111_11111111_11111111_1L",
        errLitLongTooBig());
    CHECK_TOKENS("0b0101a01L", errLitBinaryInvalidChar());
    CHECK_TOKENS("0b0好0L", errLitBinaryInvalidChar());
    CHECK_TOKENS("0b01\a001L", errLitBinaryInvalidChar());
  }

  SECTION("Spans") {
    CHECK_SPANS(" 123L ", input::Span{1, 4});
    CHECK_SPANS(" 123L  0L", input::Span{1, 4}, input::Span{7, 8});
    CHECK_SPANS("1__2__3L", input::Span{0, 7});
    CHECK_SPANS("99999999999999999999999999L", input::Span{0, 26});
    CHECK_SPANS("12a12L", input::Span{0, 5});
    CHECK_SPANS("11____L", input::Span{0, 6});

    CHECK_SPANS(" 0x1F2F3L ", input::Span{1, 8});
    CHECK_SPANS(" 0xF1A2C3L  0xL", input::Span{1, 9}, input::Span{12, 14});
    CHECK_SPANS("0xA__B__CL", input::Span{0, 9});
    CHECK_SPANS("0x123456789ABCDEFL", input::Span{0, 17});
    CHECK_SPANS("0xtestL", input::Span{0, 6});
    CHECK_SPANS("0xa____L", input::Span{0, 7});

    CHECK_SPANS(" 0b1L ", input::Span{1, 4});
    CHECK_SPANS(" 0b101L  0bL", input::Span{1, 6}, input::Span{9, 11});
    CHECK_SPANS("0b1__1__1l", input::Span{0, 9});
    CHECK_SPANS("0b1010101101L", input::Span{0, 12});
    CHECK_SPANS("0btestL", input::Span{0, 6});
    CHECK_SPANS("0b1____l", input::Span{0, 7});
  }
}

} // namespace lex
