#include "lex/error.hpp"
#include <string>

namespace lex {

auto errInvalidChar(const char c, const input::Span span) -> Token {
  return errorToken("Invalid character '" + std::string(1, c) + "'", span);
}

auto errLitIntTooBig(const input::Span span) -> Token {
  return errorToken("Integer literal too big", span);
}

auto errLitLongTooBig(const input::Span span) -> Token {
  return errorToken("Long literal too big", span);
}

auto errLitFloatUnrepresentable(const input::Span span) -> Token {
  return errorToken("Literal not representable by a floating-point number", span);
}

auto errLitNumberInvalidChar(const input::Span span) -> Token {
  return errorToken("Number literal contains an invalid character", span);
}

auto errLitHexInvalidChar(const input::Span span) -> Token {
  return errorToken("Hex integer literal contains an invalid character", span);
}

auto errLitBinaryInvalidChar(const input::Span span) -> Token {
  return errorToken("Binary integer literal contains an invalid character", span);
}

auto errLitNumberEndsWithSeperator(const input::Span span) -> Token {
  return errorToken("Number literal ends with a seperator character", span);
}

auto errLitNumberEndsWithDecimalPoint(const input::Span span) -> Token {
  return errorToken("Number literal ends with a decimal point character", span);
}

auto erLitStrUnterminated(const input::Span span) -> Token {
  return errorToken("Unterminated string literal", span);
}

auto errLitStrInvalidEscape(const input::Span span) -> Token {
  return errorToken("Invalid escape sequence in string literal", span);
}

auto erLitCharEmpty(const input::Span span) -> Token {
  return errorToken("Empty character literal", span);
}

auto errLitCharTooBig(const input::Span span) -> Token {
  return errorToken("Character literal too big", span);
}

auto errLitCharUnterminated(const input::Span span) -> Token {
  return errorToken("Unterminated character literal", span);
}

auto errLitCharInvalidEscape(const input::Span span) -> Token {
  return errorToken("Invalid escape sequence in character literal", span);
}

auto errStaticIntNoDigits(const input::Span span) -> Token {
  return errorToken("Static integer needs at least one digit", span);
}

auto errStaticIntTooBig(const input::Span span) -> Token {
  return errorToken("Static integer too big", span);
}

auto errStaticIntInvalidChar(const input::Span span) -> Token {
  return errorToken("Static integer contains an invalid character", span);
}

auto errIdentifierIllegalCharacter(const input::Span span) -> Token {
  return errorToken("Identifier contains an invalid character", span);
}

auto errIdentifierIllegalSequence(const input::Span span) -> Token {
  return errorToken("Identifier contains an illegal character sequence", span);
}

} // namespace lex
