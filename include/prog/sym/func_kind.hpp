#pragma once

namespace prog::sym {

enum class FuncKind {
  NoOp,
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

  DefInt,
  DefFloat,
  DefBool,
  DefString,

  MakeStruct,
  MakeUnion,

  CheckEqUserType,
  CheckNEqUserType,
};

} // namespace prog::sym
