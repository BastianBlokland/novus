#pragma once
#include "lex/token.hpp"

namespace lex {

auto errInvalidChar(char c, SourceSpan span = SourceSpan{0}) -> Token;

auto errLitIntTooBig(SourceSpan span = SourceSpan{0}) -> Token;

auto errLitIntInvalidChar(SourceSpan span = SourceSpan{0}) -> Token;

auto errLitIntEndsWithSeperator(SourceSpan span = SourceSpan{0}) -> Token;

auto erLitStrUnterminated(SourceSpan span = SourceSpan{0}) -> Token;

auto errLitStrInvalidEscape(SourceSpan span = SourceSpan{0}) -> Token;

auto errIdentifierIllegalCharacter(SourceSpan span = SourceSpan{0}) -> Token;

auto errIdentifierIllegalSequence(SourceSpan span = SourceSpan{0}) -> Token;

} // namespace lex
