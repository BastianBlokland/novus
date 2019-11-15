#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class OpCode : uint8_t {
  LoadLitInt = 10,

  StoreConst = 21,
  LoadConst  = 22,

  AddInt      = 50,
  SubInt      = 51,
  MulInt      = 52,
  DivInt      = 53,
  NegInt      = 54,
  LogicInvInt = 55,

  CheckEqInt = 60,
  CheckGtInt = 61,
  CheckLeInt = 62,

  PrintInt   = 100,
  PrintLogic = 101,

  Jump   = 220,
  JumpIf = 221,

  Call = 230,
  Ret  = 231,

  Fail = 255,
};

auto operator<<(std::ostream& out, const OpCode& rhs) -> std::ostream&;

} // namespace vm
