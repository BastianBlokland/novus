#pragma once
#include "lex/token.hpp"

namespace lex {

[[nodiscard]] auto errInvalidChar(char c, input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitIntTooBig(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitLongTooBig(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitFloatUnrepresentable(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitNumberInvalidChar(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitHexInvalidChar(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitBinaryInvalidChar(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitNumberEndsWithSeperator(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitNumberEndsWithDecimalPoint(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto erLitStrUnterminated(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitStrInvalidEscape(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto erLitCharEmpty(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitCharTooBig(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitCharUnterminated(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errLitCharInvalidEscape(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errStaticIntNoDigits(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errStaticIntTooBig(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errStaticIntInvalidChar(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errIdentifierIllegalCharacter(input::Span span = input::Span{0}) -> Token;

[[nodiscard]] auto errIdentifierIllegalSequence(input::Span span = input::Span{0}) -> Token;

} // namespace lex
