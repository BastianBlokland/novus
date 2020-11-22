#pragma once
#include <string>

namespace prog {

// Supported operators.
// Note: Operators are just plain functions with a specific name in the program.
// Use the 'getFuncName' to find the mangled name of an operator.
enum class Operator {
  Plus,
  PlusPlus,
  Minus,
  MinusMinus,
  Star,
  Slash,
  Rem,
  Amp,
  Pipe,
  ShiftL,
  ShiftR,
  Hat,
  Tilde,
  EqEq,
  Bang,
  BangEq,
  Le,
  LeEq,
  Gt,
  GtEq,
  ColonColon,
  SquareSquare,
  ParenParen,
  QMark,
  QMarkQMark,
};

// Retrieve the mangled name of an operator.
auto getFuncName(Operator op) -> std::string;

} // namespace prog
