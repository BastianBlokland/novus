#pragma once

namespace prog::sym {

enum class FuncKind {
  User,

  NegateInt,
  AddInt,
  SubInt,
  MulInt,
  DivInt,
  CheckEqInt,
  CheckNEqInt,
  CheckLessInt,
  CheckLessEqInt,
  CheckGreaterInt,
  CheckGreaterEqInt,

  InvBool,
  CheckEqBool,
  CheckNEqBool,
};

} // namespace prog::sym
