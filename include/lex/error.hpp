#pragma once
#include "lex/token.hpp"
#include <string>

namespace lex {

auto errInvalidChar(const char c, const SourceSpan span = SourceSpan{0}) -> Token;

auto errLitIntTooBig(const SourceSpan span = SourceSpan{0}) -> Token;

auto errLitIntInvalidChar(const SourceSpan span = SourceSpan{0}) -> Token;

auto errLitIntEndsWithSeperator(const SourceSpan span = SourceSpan{0}) -> Token;

auto erLitStrUnterminated(const SourceSpan span = SourceSpan{0}) -> Token;

auto errLitStrInvalidEscape(const SourceSpan span = SourceSpan{0}) -> Token;

auto errIdentifierIllegalCharacter(const SourceSpan span = SourceSpan{0}) -> Token;

auto errIdentifierIllegalSequence(const SourceSpan span = SourceSpan{0}) -> Token;

} // namespace lex
