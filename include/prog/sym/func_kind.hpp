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
  CheckLeInt,
  CheckLeEqInt,
  CheckGtInt,
  CheckGtEqInt,

  InvBool,
  CheckEqBool,
  CheckNEqBool,
};

} // namespace prog::sym
