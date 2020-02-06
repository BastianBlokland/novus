#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class OpCode : uint8_t {
  LoadLitInt      = 10,
  LoadLitIntSmall = 11,
  LoadLitInt0     = 12,
  LoadLitInt1     = 13,
  LoadLitFloat    = 14,
  LoadLitString   = 15,

  LoadLitIp = 20,

  StackAlloc = 30,
  StackStore = 31,
  StackLoad  = 32,

  AddInt        = 40,
  AddFloat      = 41,
  AddString     = 42,
  SubInt        = 43,
  SubFloat      = 44,
  MulInt        = 45,
  MulFloat      = 46,
  DivInt        = 47,
  DivFloat      = 48,
  RemInt        = 49,
  ModFloat      = 50,
  PowFloat      = 51,
  SqrtFloat     = 52,
  SinFloat      = 53,
  CosFloat      = 54,
  TanFloat      = 55,
  ASinFloat     = 56,
  ACosFloat     = 57,
  ATanFloat     = 58,
  ATan2Float    = 59,
  NegInt        = 60,
  NegFloat      = 61,
  LogicInvInt   = 62,
  ShiftLeftInt  = 63,
  ShiftRightInt = 64,
  AndInt        = 65,
  OrInt         = 66,
  XorInt        = 67,
  InvInt        = 68,
  LengthString  = 69,
  IndexString   = 70,
  SliceString   = 71,

  CheckEqInt        = 80,
  CheckEqFloat      = 81,
  CheckEqString     = 82,
  CheckEqIp         = 83,
  CheckEqCallDynTgt = 84,
  CheckGtInt        = 85,
  CheckGtFloat      = 86,
  CheckLeInt        = 87,
  CheckLeFloat      = 88,

  ConvIntFloat    = 91,
  ConvFloatInt    = 92,
  ConvIntString   = 93,
  ConvFloatString = 94,
  ConvCharString  = 95,
  ConvIntChar     = 96,

  MakeStruct      = 100,
  LoadStructField = 101,

  Jump   = 220,
  JumpIf = 221,

  Call        = 230,
  CallTail    = 231,
  CallDyn     = 232,
  CallDynTail = 233,
  PCall       = 234,
  Ret         = 235,

  Dup = 240,
  Pop = 241,

  Fail = 255,
};

auto operator<<(std::ostream& out, const OpCode& rhs) noexcept -> std::ostream&;

} // namespace vm
