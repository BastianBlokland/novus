#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class OpCode : uint8_t {
  LoadLitInt      = 10,
  LoadLitIntSmall = 11,
  LoadLitInt0     = 12,
  LoadLitInt1     = 13,

  LoadLitFloat  = 20,
  LoadLitString = 21,

  LoadLitIp = 30,

  ReserveConsts = 40,
  StoreConst    = 41,
  LoadConst     = 42,

  AddInt        = 50,
  AddFloat      = 51,
  AddString     = 52,
  SubInt        = 53,
  SubFloat      = 54,
  MulInt        = 55,
  MulFloat      = 56,
  DivInt        = 57,
  DivFloat      = 58,
  RemInt        = 59,
  NegInt        = 60,
  NegFloat      = 61,
  LogicInvInt   = 62,
  ShiftLeftInt  = 63,
  ShiftRightInt = 64,
  AndInt        = 65,
  OrInt         = 66,
  XorInt        = 67,
  LengthString  = 68,

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

  PrintString = 150,

  Jump   = 220,
  JumpIf = 221,

  Call        = 230,
  CallTail    = 231,
  CallDyn     = 232,
  CallDynTail = 233,
  Ret         = 234,

  Dup = 240,
  Pop = 241,

  Fail = 255,
};

auto operator<<(std::ostream& out, const OpCode& rhs) -> std::ostream&;

} // namespace vm
