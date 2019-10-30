#pragma once
#include "lex/token.hpp"

namespace lex {

auto errInvalidChar(char c, input::SourceSpan span = input::SourceSpan{0}) -> Token;

auto errLitIntTooBig(input::SourceSpan span = input::SourceSpan{0}) -> Token;

auto errLitIntInvalidChar(input::SourceSpan span = input::SourceSpan{0}) -> Token;

auto errLitIntEndsWithSeperator(input::SourceSpan span = input::SourceSpan{0}) -> Token;

auto erLitStrUnterminated(input::SourceSpan span = input::SourceSpan{0}) -> Token;

auto errLitStrInvalidEscape(input::SourceSpan span = input::SourceSpan{0}) -> Token;

auto errIdentifierIllegalCharacter(input::SourceSpan span = input::SourceSpan{0}) -> Token;

auto errIdentifierIllegalSequence(input::SourceSpan span = input::SourceSpan{0}) -> Token;

} // namespace lex
