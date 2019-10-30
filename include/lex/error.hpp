#pragma once
#include "lex/token.hpp"

namespace lex {

auto errInvalidChar(char c, input::Span span = input::Span{0}) -> Token;

auto errLitIntTooBig(input::Span span = input::Span{0}) -> Token;

auto errLitIntInvalidChar(input::Span span = input::Span{0}) -> Token;

auto errLitIntEndsWithSeperator(input::Span span = input::Span{0}) -> Token;

auto erLitStrUnterminated(input::Span span = input::Span{0}) -> Token;

auto errLitStrInvalidEscape(input::Span span = input::Span{0}) -> Token;

auto errIdentifierIllegalCharacter(input::Span span = input::Span{0}) -> Token;

auto errIdentifierIllegalSequence(input::Span span = input::Span{0}) -> Token;

} // namespace lex
