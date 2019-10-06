#include "lex/error.hpp"
#include <string>

namespace lex {

auto errInvalidChar(const char c, const SourceSpan span) -> Token {
  return errorToken("Invalid character." + std::string(1, c) + "'.", span);
}

auto errLitIntTooBig(const SourceSpan span) -> Token {
  return errorToken("Integer literal too big.", span);
}

auto errLitIntInvalidChar(const SourceSpan span) -> Token {
  return errorToken("Integer literal contains an invalid character.", span);
}

auto errLitIntEndsWithSeperator(const SourceSpan span) -> Token {
  return errorToken("Integer literal ends with a seperator character.", span);
}

auto erLitStrUnterminated(const SourceSpan span) -> Token {
  return errorToken("Unterminated string literal.", span);
}

auto errLitStrInvalidEscape(const SourceSpan span) -> Token {
  return errorToken("Invalid escape sequence in string literal.", span);
}

auto errIdentifierIllegalCharacter(const SourceSpan span) -> Token {
  return errorToken("Identifier contains an invalid character.", span);
}

auto errIdentifierIllegalSequence(const SourceSpan span) -> Token {
  return errorToken("Identifier contains an illegal character sequence.", span);
}

} // namespace lex
