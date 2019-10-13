#pragma once
#include "lex/token.hpp"

namespace parse {

auto getUnaryOpPrecedence(const lex::Token& token) -> int;

auto getBinaryOpPrecedence(const lex::Token& token) -> int;

} // namespace parse
