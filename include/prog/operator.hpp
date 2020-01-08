#pragma once
#include <string>

namespace prog {

enum class Operator {
  Plus,
  Minus,
  Star,
  Slash,
  Rem,
  Amp,
  Pipe,
  ShiftL,
  ShiftR,
  Hat,
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
  QMarkQMark,
};

auto getFuncName(Operator op) -> std::string;

} // namespace prog
