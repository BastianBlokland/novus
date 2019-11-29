#include "vm/opcode.hpp"

namespace vm {

auto operator<<(std::ostream& out, const OpCode& rhs) -> std::ostream& {
  switch (rhs) {
  case OpCode::LoadLitInt:
    out << "load-int";
    break;
  case OpCode::LoadLitString:
    out << "load-string";
    break;

  case OpCode::ReserveConsts:
    out << "reserve-consts";
    break;
  case OpCode::StoreConst:
    out << "store-const";
    break;
  case OpCode::LoadConst:
    out << "load-const";
    break;

  case OpCode::AddInt:
    out << "add-int";
    break;
  case OpCode::AddString:
    out << "add-string";
    break;
  case OpCode::SubInt:
    out << "sub-int";
    break;
  case OpCode::MulInt:
    out << "mul-int";
    break;
  case OpCode::DivInt:
    out << "div-int";
    break;
  case OpCode::RemInt:
    out << "rem-int";
    break;
  case OpCode::NegInt:
    out << "neg-int";
    break;
  case OpCode::LogicInvInt:
    out << "logicinvert-int";
    break;

  case OpCode::CheckEqInt:
    out << "check-eq-int";
    break;
  case OpCode::CheckEqString:
    out << "check-eq-string";
    break;
  case OpCode::CheckGtInt:
    out << "check-gt-int";
    break;
  case OpCode::CheckLeInt:
    out << "check-le-int";
    break;

  case OpCode::ConvIntString:
    out << "conv-int-string";
    break;

  case OpCode::PrintString:
    out << "print-string";
    break;

  case OpCode::Jump:
    out << "jump";
    break;
  case OpCode::JumpIf:
    out << "jump-if";
    break;

  case OpCode::Call:
    out << "call";
    break;
  case OpCode::Ret:
    out << "ret";
    break;

  case OpCode::Fail:
    out << "fail";
    break;
  }
  return out;
}

} // namespace vm
