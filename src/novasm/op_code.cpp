#include "novasm/op_code.hpp"

namespace novasm {

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
  case OpCode::LoadLitLong:
    out << "load-long";
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
  case OpCode::StackAllocSmall:
    out << "stack-alloc-small";
    break;
  case OpCode::StackStore:
    out << "stack-store";
    break;
  case OpCode::StackStoreSmall:
    out << "stack-store-small";
    break;
  case OpCode::StackLoad:
    out << "stack-load";
    break;
  case OpCode::StackLoadSmall:
    out << "stack-load-small";
    break;

  case OpCode::AddInt:
    out << "add-int";
    break;
  case OpCode::AddLong:
    out << "add-long";
    break;
  case OpCode::AddFloat:
    out << "add-float";
    break;
  case OpCode::AddString:
    out << "add-string";
    break;
  case OpCode::AppendChar:
    out << "append-char";
    break;
  case OpCode::SubInt:
    out << "sub-int";
    break;
  case OpCode::SubLong:
    out << "sub-long";
    break;
  case OpCode::SubFloat:
    out << "sub-float";
    break;
  case OpCode::MulInt:
    out << "mul-int";
    break;
  case OpCode::MulLong:
    out << "mul-long";
    break;
  case OpCode::MulFloat:
    out << "mul-float";
    break;
  case OpCode::DivInt:
    out << "div-int";
    break;
  case OpCode::DivLong:
    out << "div-long";
    break;
  case OpCode::DivFloat:
    out << "div-float";
    break;
  case OpCode::RemInt:
    out << "rem-int";
    break;
  case OpCode::RemLong:
    out << "rem-long";
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
  case OpCode::NegLong:
    out << "neg-long";
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
  case OpCode::ShiftLeftLong:
    out << "shiftleft-long";
    break;
  case OpCode::ShiftRightInt:
    out << "shiftright-int";
    break;
  case OpCode::ShiftRightLong:
    out << "shiftright-long";
    break;
  case OpCode::AndInt:
    out << "and-int";
    break;
  case OpCode::AndLong:
    out << "and-long";
    break;
  case OpCode::OrInt:
    out << "or-int";
    break;
  case OpCode::OrLong:
    out << "or-long";
    break;
  case OpCode::XorInt:
    out << "xor-int";
    break;
  case OpCode::XorLong:
    out << "xor-long";
    break;
  case OpCode::InvInt:
    out << "inv-int";
    break;
  case OpCode::InvLong:
    out << "inv-long";
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
  case OpCode::CheckEqLong:
    out << "check-eq-long";
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
  case OpCode::CheckGtLong:
    out << "check-gt-long";
    break;
  case OpCode::CheckGtFloat:
    out << "check-gt-float";
    break;
  case OpCode::CheckLeInt:
    out << "check-le-int";
    break;
  case OpCode::CheckLeLong:
    out << "check-le-long";
    break;
  case OpCode::CheckLeFloat:
    out << "check-le-float";
    break;
  case OpCode::CheckStructNull:
    out << "check-struct-null";
    break;

  case OpCode::ConvIntLong:
    out << "conv-int-long";
    break;
  case OpCode::ConvIntFloat:
    out << "conv-int-float";
    break;
  case OpCode::ConvLongInt:
    out << "conv-long-int";
    break;
  case OpCode::ConvLongFloat:
    out << "conv-long-float";
    break;
  case OpCode::ConvFloatInt:
    out << "conv-float-int";
    break;
  case OpCode::ConvIntString:
    out << "conv-int-string";
    break;
  case OpCode::ConvLongString:
    out << "conv-long-string";
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
  case OpCode::ConvLongChar:
    out << "conv-long-char";
    break;
  case OpCode::ConvFloatChar:
    out << "conv-float-char";
    break;
  case OpCode::ConvFloatLong:
    out << "conv-float-long";
    break;

  case OpCode::MakeAtomic:
    out << "make-atomic";
    break;
  case OpCode::AtomicLoad:
    out << "atomic-load";
    break;
  case OpCode::AtomicCompareSwap:
    out << "atomic-compare-swap";
    break;
  case OpCode::AtomicBlock:
    out << "atomic-block";
    break;

  case OpCode::MakeStruct:
    out << "make-struct";
    break;
  case OpCode::MakeNullStruct:
    out << "make-null-struct";
    break;
  case OpCode::StructLoadField:
    out << "struct-load-field";
    break;
  case OpCode::StructStoreField:
    out << "struct-store-field";
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
  case OpCode::CallForked:
    out << "call-forked";
    break;
  case OpCode::CallDyn:
    out << "call-dyn";
    break;
  case OpCode::CallDynTail:
    out << "call-dyn-tail";
    break;
  case OpCode::CallDynForked:
    out << "call-dyn-forked";
    break;
  case OpCode::PCall:
    out << "pcall";
    break;
  case OpCode::Ret:
    out << "ret";
    break;

  case OpCode::FutureWaitNano:
    out << "future-wait-nano";
    break;
  case OpCode::FutureBlock:
    out << "future-block";
    break;
  case OpCode::Dup:
    out << "dup";
    break;
  case OpCode::Pop:
    out << "pop";
    break;
  case OpCode::Swap:
    out << "swap";
    break;

  case OpCode::Fail:
    out << "fail";
    break;
  }
  return out;
}

} // namespace novasm
