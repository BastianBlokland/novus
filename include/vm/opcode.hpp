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

  AddInt      = 50,
  AddFloat    = 51,
  AddString   = 52,
  SubInt      = 53,
  SubFloat    = 54,
  MulInt      = 55,
  MulFloat    = 56,
  DivInt      = 57,
  DivFloat    = 58,
  RemInt      = 59,
  NegInt      = 60,
  NegFloat    = 61,
  LogicInvInt = 62,

  CheckEqInt        = 70,
  CheckEqFloat      = 71,
  CheckEqString     = 72,
  CheckEqIp         = 73,
  CheckEqCallDynTgt = 74,
  CheckGtInt        = 75,
  CheckGtFloat      = 76,
  CheckLeInt        = 77,
  CheckLeFloat      = 78,

  ConvIntFloat    = 81,
  ConvFloatInt    = 82,
  ConvIntString   = 83,
  ConvFloatString = 84,

  MakeStruct      = 90,
  LoadStructField = 91,

  PrintString = 100,

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
