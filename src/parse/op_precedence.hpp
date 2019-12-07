#pragma once
#include "lex/token.hpp"

namespace parse {

const int dotPrecendece            = 12;
const int unaryPrecedence          = 11;
const int multiplicativePrecedence = 10;
const int additivePrecedence       = 9;
const int typeTestPrecedence       = 8;
const int relationalPrecedence     = 7;
const int equalityPrecedence       = 6;
const int logicAndPrecedence       = 5;
const int logicOrPrecedence        = 4;
const int conditionalPrecedence    = 3;
const int assignmentPrecedence     = 2;
const int groupingPrecedence       = 1;

auto getLhsOpPrecedence(const lex::Token& token) -> int;

auto getRhsOpPrecedence(const lex::Token& token) -> int;

} // namespace parse
