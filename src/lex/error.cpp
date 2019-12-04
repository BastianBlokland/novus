#include "lex/error.hpp"
#include <string>

namespace lex {

auto errInvalidChar(const char c, const input::Span span) -> Token {
  return errorToken("Invalid character '" + std::string(1, c) + "'", span);
}

auto errLitIntTooBig(const input::Span span) -> Token {
  return errorToken("Integer literal too big", span);
}

auto errLitFloatUnrepresentable(const input::Span span) -> Token {
  return errorToken("Literal not representable by a floating-point number", span);
}

auto errLitNumberInvalidChar(const input::Span span) -> Token {
  return errorToken("Number literal contains an invalid character", span);
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

auto errIdentifierIllegalCharacter(const input::Span span) -> Token {
  return errorToken("Identifier contains an invalid character", span);
}

auto errIdentifierIllegalSequence(const input::Span span) -> Token {
  return errorToken("Identifier contains an illegal character sequence", span);
}

} // namespace lex
