#include "lex/error.hpp"

namespace lex {

auto errInvalidCharacter(const char c, const SourceSpan span) -> Token {
  return errorToken("Invalid character '" + std::to_string(c) + "'.", span);
}

auto errLitIntTooBig(const SourceSpan span) -> Token {
  return errorToken("Integer literal too big.", span);
}

auto errLitIntInvalidChar(const SourceSpan span) -> Token {
  return errorToken("Integer literal contains an invalid character.", span);
}

auto errUnterminatedStringLiteral(const SourceSpan span) -> Token {
  return errorToken("Unterminated string literal.", span);
}

} // namespace lex
