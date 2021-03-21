#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"

namespace lex {

TEST_CASE("[lex] Lexing static integers", "lex") {

  SECTION("Single values") {
    CHECK_TOKENS("#0", staticIntToken(0));
    CHECK_TOKENS("#42", staticIntToken(42));
    CHECK_TOKENS("#-42", staticIntToken(-42));
    CHECK_TOKENS("#0042", staticIntToken(42));
    CHECK_TOKENS("#123456", staticIntToken(123456));
    CHECK_TOKENS("#2147483647", staticIntToken(2147483647));
  }

  SECTION("Sequences") {
    CHECK_TOKENS("#0 #1 #-1234", staticIntToken(0), staticIntToken(1), staticIntToken(-1234));
    CHECK_TOKENS("#1,#2", staticIntToken(1), basicToken(TokenKind::SepComma), staticIntToken(2));
  }

  SECTION("Errors") {
    CHECK_TOKENS("#", errStaticIntNoDigits());
    CHECK_TOKENS("#2147483648", errStaticIntTooBig());
    CHECK_TOKENS("#99999999999999999999", errStaticIntTooBig());
    CHECK_TOKENS("#13a4a2", errStaticIntInvalidChar());
    CHECK_TOKENS("#--13", errStaticIntInvalidChar());
    CHECK_TOKENS("#13a4好2", errStaticIntInvalidChar());
    CHECK_TOKENS("#13a4\a2", errStaticIntInvalidChar());
  }

  SECTION("Spans") {
    CHECK_SPANS(" #123 ", input::Span{1, 4});
    CHECK_SPANS(" #123  #0", input::Span{1, 4}, input::Span{7, 8});
  }
}

} // namespace lex
