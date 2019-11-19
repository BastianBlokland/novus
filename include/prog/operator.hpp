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
  EqEq,
  Bang,
  BangEq,
  Le,
  LeEq,
  Gt,
  GtEq,
};

auto getFuncName(Operator op) -> std::string;

} // namespace prog
