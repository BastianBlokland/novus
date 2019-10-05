#pragma once
#include "lex/token.hpp"
#include <string>

namespace lex {

auto errInvalidCharacter(const SourceSpan span, const char c) {
  return errorToken(span, "Invalid character '" + std::to_string(c) + "'.");
}

auto errTooBigIntLiteral(const SourceSpan span) {
  return errorToken(span, "Integer literal too big.");
}

auto errUnterminatedStringLiteral(const SourceSpan span) {
  return errorToken(span, "Unterminated string literal.");
}

} // namespace lex
