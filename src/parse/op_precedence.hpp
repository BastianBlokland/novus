#pragma once
#include "lex/token.hpp"

namespace parse {

const int callPrecedence           = 17;
const int fieldPrecedence          = 16;
const int doubleQmarkPrecedence    = 15;
const int unaryPrecedence          = 14;
const int multiplicativePrecedence = 13;
const int additivePrecedence       = 12;
const int shiftPrecedence          = 11;
const int typeTestPrecedence       = 10;
const int relationalPrecedence     = 9;
const int equalityPrecedence       = 8;
const int andPrecedence            = 7;
const int xorPrecedence            = 6;
const int orPrecedence             = 5;
const int bangPrecedence           = 4;
const int conditionalPrecedence    = 3;
const int assignmentPrecedence     = 2;
const int groupingPrecedence       = 1;

auto getLhsOpPrecedence(const lex::Token& token) -> int;

auto getRhsOpPrecedence(const lex::Token& token) -> int;

auto isRightAssociative(const lex::Token& token) -> bool;

} // namespace parse
