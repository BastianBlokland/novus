#pragma once
#include "lex/token.hpp"
#include <string>

namespace lex {

auto errInvalidCharacter(const char c, const SourceSpan span = SourceSpan{0}) -> Token;

auto errLitIntTooBig(const SourceSpan span = SourceSpan{0}) -> Token;

auto errLitIntInvalidChar(const SourceSpan span = SourceSpan{0}) -> Token;

auto errLitIntEndsWithSeperator(const SourceSpan span = SourceSpan{0}) -> Token;

auto errUnterminatedStringLiteral(const SourceSpan span = SourceSpan{0}) -> Token;

} // namespace lex
