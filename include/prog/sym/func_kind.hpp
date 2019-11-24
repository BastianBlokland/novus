#pragma once

namespace prog::sym {

enum class FuncKind {
  User,

  NegateInt,
  AddInt,
  SubInt,
  MulInt,
  DivInt,
  RemInt,
  CheckEqInt,
  CheckNEqInt,
  CheckLeInt,
  CheckLeEqInt,
  CheckGtInt,
  CheckGtEqInt,

  InvBool,
  CheckEqBool,
  CheckNEqBool,

  ConvIntString,
  ConvBoolString,
};

} // namespace prog::sym
