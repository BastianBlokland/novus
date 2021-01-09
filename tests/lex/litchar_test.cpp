#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "lex/error.hpp"

namespace lex {

TEST_CASE("[lex] Lexing character literals", "lex") {

  SECTION("Single values") {
    CHECK_TOKENS("'a'", litCharToken('a'));
    CHECK_TOKENS("'\"'", litCharToken('"'));
    CHECK_TOKENS("'0'", litCharToken('0'));
    CHECK_TOKENS("';'", litCharToken(';'));
    CHECK_TOKENS("'.'", litCharToken('.'));
    CHECK_TOKENS("'}'", litCharToken('}'));
    CHECK_TOKENS("'?'", litCharToken('?'));
    CHECK_TOKENS("'?'", litCharToken('?'));
    CHECK_TOKENS("' '", litCharToken(' '));
  }

  SECTION("Escaping") {
    CHECK_TOKENS("'\\''", litCharToken('\''));
    CHECK_TOKENS("'\\\"'", litCharToken('"'));
    CHECK_TOKENS("'\\''", litCharToken('\''));
    CHECK_TOKENS("'\\\\'", litCharToken('\\'));
    CHECK_TOKENS("'\\0'", litCharToken(0));
    CHECK_TOKENS("'\\a'", litCharToken('\a'));
    CHECK_TOKENS("'\\b'", litCharToken('\b'));
    CHECK_TOKENS("'\\f'", litCharToken('\f'));
    CHECK_TOKENS("'\\n'", litCharToken('\n'));
    CHECK_TOKENS("'\\r'", litCharToken('\r'));
    CHECK_TOKENS("'\\t'", litCharToken('\t'));
    CHECK_TOKENS("'\\v'", litCharToken('\v'));
  }

  SECTION("Sequences") {
    CHECK_TOKENS(
        "'h' 'e' 'l''l''o'",
        litCharToken('h'),
        litCharToken('e'),
        litCharToken('l'),
        litCharToken('l'),
        litCharToken('o'));
    CHECK_TOKENS("'/','.'", litCharToken('/'), basicToken(TokenKind::SepComma), litCharToken('.'));
  }

  SECTION("Errors") {
    CHECK_TOKENS("''", erLitCharEmpty());
    CHECK_TOKENS("'aa'", errLitCharTooBig());
    CHECK_TOKENS("'€'", errLitCharTooBig());
    CHECK_TOKENS("'你'", errLitCharTooBig());
    CHECK_TOKENS("'\\g'", errLitCharInvalidEscape());
    CHECK_TOKENS("'\\1'", errLitCharInvalidEscape());
    CHECK_TOKENS("'", errLitCharUnterminated());
    CHECK_TOKENS("'aaaa", errLitCharUnterminated());
    CHECK_TOKENS("'a\n", errLitCharUnterminated());
  }

  SECTION("Spans") {
    CHECK_SPANS(" 'a' ", input::Span{1, 3});
    CHECK_SPANS(" 'asdfsd' ", input::Span{1, 8});
    CHECK_SPANS(" 'asdf ", input::Span{1, 6});
  }
}

} // namespace lex
