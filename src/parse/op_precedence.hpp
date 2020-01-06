#pragma once
#include "lex/token.hpp"

namespace parse {

const int callPrecedence           = 14;
const int unaryPrecedence          = 13;
const int multiplicativePrecedence = 12;
const int additivePrecedence       = 11;
const int shiftPrecedence          = 10;
const int typeTestPrecedence       = 9;
const int relationalPrecedence     = 8;
const int equalityPrecedence       = 7;
const int andPrecedence            = 6;
const int xorPrecedence            = 5;
const int orPrecedence             = 4;
const int conditionalPrecedence    = 3;
const int assignmentPrecedence     = 2;
const int groupingPrecedence       = 1;

auto getLhsOpPrecedence(const lex::Token& token) -> int;

auto getRhsOpPrecedence(const lex::Token& token) -> int;

auto isRightAssociative(const lex::Token& token) -> bool;

} // namespace parse
