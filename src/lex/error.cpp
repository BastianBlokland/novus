#include "lex/error.hpp"
#include <string>

namespace lex {

auto errInvalidChar(const char c, const input::Span span) -> Token {
  return errorToken("Invalid character '" + std::string(1, c) + "'", span);
}

auto errLitIntTooBig(const input::Span span) -> Token {
  return errorToken("Integer literal too big", span);
}

auto errLitIntInvalidChar(const input::Span span) -> Token {
  return errorToken("Integer literal contains an invalid character", span);
}

auto errLitIntEndsWithSeperator(const input::Span span) -> Token {
  return errorToken("Integer literal ends with a seperator character", span);
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
