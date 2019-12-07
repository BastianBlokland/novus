#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class OpCode : uint8_t {
  LoadLitInt    = 10,
  LoadLitFloat  = 11,
  LoadLitString = 12,

  ReserveConsts = 20,
  StoreConst    = 21,
  LoadConst     = 22,

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

  CheckEqInt    = 70,
  CheckEqFloat  = 71,
  CheckEqString = 72,
  CheckGtInt    = 73,
  CheckGtFloat  = 74,
  CheckLeInt    = 75,
  CheckLeFloat  = 76,

  ConvIntFloat    = 81,
  ConvFloatInt    = 82,
  ConvIntString   = 83,
  ConvFloatString = 84,

  MakeStruct      = 90,
  LoadStructField = 91,

  PrintString = 100,

  Jump   = 220,
  JumpIf = 221,

  Call = 230,
  Ret  = 231,

  Dup = 240,
  Pop = 241,

  Fail = 255,
};

auto operator<<(std::ostream& out, const OpCode& rhs) -> std::ostream&;

} // namespace vm
