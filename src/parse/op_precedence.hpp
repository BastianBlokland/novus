#pragma once
#include "lex/token.hpp"

namespace parse {

const int unaryPrecedence          = 9;
const int multiplicativePrecedence = 8;
const int additivePrecedence       = 7;
const int relationalPrecedence     = 6;
const int equalityPrecedence       = 5;
const int logicAndPrecedence       = 4;
const int logicOrPrecedence        = 3;
const int assignmentPrecedence     = 2;
const int groupingPrecedence       = 1;

auto getUnaryOpPrecedence(const lex::Token& token) -> int;

auto getBinaryOpPrecedence(const lex::Token& token) -> int;

} // namespace parse
