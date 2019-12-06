#pragma once

namespace prog::sym {

enum class FuncKind {
  User,

  AddInt,
  SubInt,
  MulInt,
  DivInt,
  RemInt,
  NegateInt,
  CheckEqInt,
  CheckNEqInt,
  CheckLeInt,
  CheckLeEqInt,
  CheckGtInt,
  CheckGtEqInt,

  AddFloat,
  SubFloat,
  MulFloat,
  DivFloat,
  NegateFloat,
  CheckEqFloat,
  CheckNEqFloat,
  CheckLeFloat,
  CheckLeEqFloat,
  CheckGtFloat,
  CheckGtEqFloat,

  InvBool,
  CheckEqBool,
  CheckNEqBool,

  AddString,
  CheckEqString,
  CheckNEqString,

  ConvIntFloat,
  ConvFloatInt,
  ConvIntString,
  ConvFloatString,
  ConvBoolString,

  MakeStruct,

  CheckEqUserType,
  CheckNEqUserType,
};

} // namespace prog::sym
