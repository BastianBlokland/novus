#pragma once
#include "lex/token.hpp"

namespace parse {

const int unaryPrecedence          = 8;
const int multiplicativePrecedence = 7;
const int additivePrecedence       = 6;
const int relationalPrecedence     = 5;
const int equalityPrecedence       = 4;
const int logicAndPrecedence       = 3;
const int logicOrPrecedence        = 2;
const int groupingPrecedence       = 1;

auto getUnaryOpPrecedence(const lex::Token& token) -> int;

auto getBinaryOpPrecedence(const lex::Token& token) -> int;

} // namespace parse
