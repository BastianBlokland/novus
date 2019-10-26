#pragma once
#include <string>

namespace prog {

enum class Operator {
  Plus,
  Minus,
  Star,
  Slash,
  Amp,
  AmpAmp,
  Pipe,
  PipePipe,
  EqEq,
  Bang,
  BangEq,
  Less,
  LessEq,
  Gt,
  GtEq,
};

auto getFuncName(Operator op) -> std::string;

} // namespace prog
