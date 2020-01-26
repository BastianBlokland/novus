#pragma once
#include <string>

namespace prog {

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
  QMarkQMark,
};

auto getFuncName(Operator op) -> std::string;

} // namespace prog
