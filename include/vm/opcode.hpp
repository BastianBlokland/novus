#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class OpCode : uint8_t {
  LoadLitInt    = 10,
  LoadLitString = 11,

  ReserveConsts = 20,
  StoreConst    = 21,
  LoadConst     = 22,

  AddInt      = 50,
  AddString   = 51,
  SubInt      = 52,
  MulInt      = 53,
  DivInt      = 54,
  RemInt      = 55,
  NegInt      = 56,
  LogicInvInt = 57,

  CheckEqInt    = 60,
  CheckEqString = 61,

  CheckGtInt = 62,
  CheckLeInt = 63,

  ConvIntString = 70,

  MakeStruct      = 80,
  LoadStructField = 81,

  PrintString = 100,

  Jump   = 220,
  JumpIf = 221,

  Call = 230,
  Ret  = 231,

  Fail = 255,
};

auto operator<<(std::ostream& out, const OpCode& rhs) -> std::ostream&;

} // namespace vm
