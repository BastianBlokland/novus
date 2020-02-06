#include "vm/op_code.hpp"

namespace vm {

auto operator<<(std::ostream& out, const OpCode& rhs) noexcept -> std::ostream& {
  switch (rhs) {
  case OpCode::LoadLitInt:
    out << "load-int";
    break;
  case OpCode::LoadLitIntSmall:
    out << "load-int-small";
    break;
  case OpCode::LoadLitInt0:
    out << "load-int-0";
    break;
  case OpCode::LoadLitInt1:
    out << "load-int-1";
    break;

  case OpCode::LoadLitFloat:
    out << "load-float";
    break;

  case OpCode::LoadLitString:
    out << "load-string";
    break;

  case OpCode::LoadLitIp:
    out << "load-ip";
    break;

  case OpCode::StackAlloc:
    out << "stack-alloc";
    break;
  case OpCode::StackStore:
    out << "stack-store";
    break;
  case OpCode::StackLoad:
    out << "stack-load";
    break;

  case OpCode::AddInt:
    out << "add-int";
    break;
  case OpCode::AddFloat:
    out << "add-float";
    break;
  case OpCode::AddString:
    out << "add-string";
    break;
  case OpCode::SubInt:
    out << "sub-int";
    break;
  case OpCode::SubFloat:
    out << "sub-float";
    break;
  case OpCode::MulInt:
    out << "mul-int";
    break;
  case OpCode::MulFloat:
    out << "mul-float";
    break;
  case OpCode::DivInt:
    out << "div-int";
    break;
  case OpCode::DivFloat:
    out << "div-float";
    break;
  case OpCode::RemInt:
    out << "rem-int";
    break;
  case OpCode::ModFloat:
    out << "mod-float";
    break;
  case OpCode::PowFloat:
    out << "pow-float";
    break;
  case OpCode::SqrtFloat:
    out << "sqrt-float";
    break;
  case OpCode::SinFloat:
    out << "sin-float";
    break;
  case OpCode::CosFloat:
    out << "cos-float";
    break;
  case OpCode::TanFloat:
    out << "tan-float";
    break;
  case OpCode::ASinFloat:
    out << "asin-float";
    break;
  case OpCode::ACosFloat:
    out << "acos-float";
    break;
  case OpCode::ATanFloat:
    out << "atan-float";
    break;
  case OpCode::ATan2Float:
    out << "atan2-float";
    break;
  case OpCode::NegInt:
    out << "neg-int";
    break;
  case OpCode::NegFloat:
    out << "neg-float";
    break;
  case OpCode::LogicInvInt:
    out << "logicinvert-int";
    break;
  case OpCode::ShiftLeftInt:
    out << "shiftleft-int";
    break;
  case OpCode::ShiftRightInt:
    out << "shiftright-int";
    break;
  case OpCode::AndInt:
    out << "and-int";
    break;
  case OpCode::OrInt:
    out << "or-int";
    break;
  case OpCode::XorInt:
    out << "xor-int";
    break;
  case OpCode::InvInt:
    out << "inv-int";
    break;
  case OpCode::LengthString:
    out << "length-string";
    break;
  case OpCode::IndexString:
    out << "index-string";
    break;
  case OpCode::SliceString:
    out << "slice-string";
    break;

  case OpCode::CheckEqInt:
    out << "check-eq-int";
    break;
  case OpCode::CheckEqFloat:
    out << "check-eq-float";
    break;
  case OpCode::CheckEqString:
    out << "check-eq-string";
    break;
  case OpCode::CheckEqIp:
    out << "check-eq-ip";
    break;
  case OpCode::CheckEqCallDynTgt:
    out << "check-eq-calldyntgt";
    break;
  case OpCode::CheckGtInt:
    out << "check-gt-int";
    break;
  case OpCode::CheckGtFloat:
    out << "check-gt-float";
    break;
  case OpCode::CheckLeInt:
    out << "check-le-int";
    break;
  case OpCode::CheckLeFloat:
    out << "check-le-float";
    break;

  case OpCode::ConvIntFloat:
    out << "conv-int-float";
    break;
  case OpCode::ConvFloatInt:
    out << "conv-float-int";
    break;
  case OpCode::ConvIntString:
    out << "conv-int-string";
    break;
  case OpCode::ConvFloatString:
    out << "conv-float-string";
    break;
  case OpCode::ConvCharString:
    out << "conv-char-string";
    break;
  case OpCode::ConvIntChar:
    out << "conv-int-char";
    break;

  case OpCode::MakeStruct:
    out << "make-struct";
    break;
  case OpCode::LoadStructField:
    out << "load-struct-field";
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
  case OpCode::CallTail:
    out << "call-tail";
    break;
  case OpCode::CallDyn:
    out << "call-dyn";
    break;
  case OpCode::CallDynTail:
    out << "call-dyn-tail";
    break;
  case OpCode::PCall:
    out << "pcall";
    break;
  case OpCode::Ret:
    out << "ret";
    break;

  case OpCode::Dup:
    out << "dup";
    break;
  case OpCode::Pop:
    out << "pop";
    break;

  case OpCode::Fail:
    out << "fail";
    break;
  }
  return out;
}

} // namespace vm
