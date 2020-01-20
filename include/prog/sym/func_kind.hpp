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
  IncrementInt,
  DecrementInt,
  ShiftLeftInt,
  ShiftRightInt,
  AndInt,
  OrInt,
  XorInt,
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
  IncrementFloat,
  DecrementFloat,
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
  LengthString,
  IndexString,
  SliceString,
  CheckEqString,
  CheckNEqString,

  IncrementChar,
  DecrementChar,

  ConvIntFloat,
  ConvFloatInt,
  ConvIntString,
  ConvFloatString,
  ConvBoolString,
  ConvCharString,
  ConvIntChar,

  DefInt,
  DefFloat,
  DefBool,
  DefString,

  MakeStruct,
  MakeUnion,

  CheckEqUserType,
  CheckNEqUserType,

  ActionPrintChar,
  ActionPrintString,
  ActionPrintStringLine,
  ActionReadChar,
  ActionReadStringLine,
  ActionSleep,
  ActionAssert,
};

} // namespace prog::sym
