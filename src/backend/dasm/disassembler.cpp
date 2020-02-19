#include "backend/dasm/disassembler.hpp"
#include "backend/dasm/instruction.hpp"
#include "vm/pcall_code.hpp"
#include <stdexcept>

namespace backend::dasm {

template <typename Type>
inline auto readAsm(const uint8_t** ip) {
  const Type v = *reinterpret_cast<const Type*>(*ip); // NOLINT: Reinterpret cast
  *ip += sizeof(Type);
  return v;
}

auto disassembleInstructions(const vm::Assembly& assembly) -> std::vector<Instruction> {
  auto result = std::vector<Instruction>{};

  for (auto* ip = assembly.getIp(0); !assembly.isEnd(ip);) {
    const auto offset = assembly.getOffset(ip);
    const auto opCode = readAsm<vm::OpCode>(&ip);
    switch (opCode) {
    case vm::OpCode::LoadLitInt:
      result.push_back(Instruction(opCode, offset, {readAsm<int32_t>(&ip)}));
      continue;
    case vm::OpCode::LoadLitLong:
      result.push_back(Instruction(opCode, offset, {readAsm<int64_t>(&ip)}));
      continue;
    case vm::OpCode::LoadLitFloat:
      result.push_back(Instruction(opCode, offset, {readAsm<float>(&ip)}));
      continue;
    case vm::OpCode::LoadLitIntSmall:
    case vm::OpCode::StackAlloc:
    case vm::OpCode::StackLoad:
    case vm::OpCode::StackStore:
    case vm::OpCode::MakeStruct:
    case vm::OpCode::LoadStructField:
    case vm::OpCode::CallDyn:
    case vm::OpCode::CallDynTail:
    case vm::OpCode::CallDynForked:
      result.push_back(Instruction(opCode, offset, {readAsm<uint8_t>(&ip)}));
      continue;
    case vm::OpCode::LoadLitInt0:
    case vm::OpCode::LoadLitInt1:
    case vm::OpCode::AddInt:
    case vm::OpCode::AddFloat:
    case vm::OpCode::AddString:
    case vm::OpCode::SubInt:
    case vm::OpCode::SubFloat:
    case vm::OpCode::MulInt:
    case vm::OpCode::MulFloat:
    case vm::OpCode::DivInt:
    case vm::OpCode::DivFloat:
    case vm::OpCode::RemInt:
    case vm::OpCode::ModFloat:
    case vm::OpCode::PowFloat:
    case vm::OpCode::SqrtFloat:
    case vm::OpCode::SinFloat:
    case vm::OpCode::CosFloat:
    case vm::OpCode::TanFloat:
    case vm::OpCode::ASinFloat:
    case vm::OpCode::ACosFloat:
    case vm::OpCode::ATanFloat:
    case vm::OpCode::ATan2Float:
    case vm::OpCode::NegInt:
    case vm::OpCode::NegFloat:
    case vm::OpCode::LogicInvInt:
    case vm::OpCode::ShiftLeftInt:
    case vm::OpCode::ShiftRightInt:
    case vm::OpCode::AndInt:
    case vm::OpCode::OrInt:
    case vm::OpCode::XorInt:
    case vm::OpCode::InvInt:
    case vm::OpCode::LengthString:
    case vm::OpCode::IndexString:
    case vm::OpCode::SliceString:
    case vm::OpCode::CheckEqInt:
    case vm::OpCode::CheckEqLong:
    case vm::OpCode::CheckEqFloat:
    case vm::OpCode::CheckEqString:
    case vm::OpCode::CheckEqIp:
    case vm::OpCode::CheckEqCallDynTgt:
    case vm::OpCode::CheckGtInt:
    case vm::OpCode::CheckGtFloat:
    case vm::OpCode::CheckLeInt:
    case vm::OpCode::CheckLeFloat:
    case vm::OpCode::ConvIntFloat:
    case vm::OpCode::ConvFloatInt:
    case vm::OpCode::ConvIntString:
    case vm::OpCode::ConvLongString:
    case vm::OpCode::ConvFloatString:
    case vm::OpCode::ConvCharString:
    case vm::OpCode::ConvIntChar:
    case vm::OpCode::Ret:
    case vm::OpCode::Fail:
    case vm::OpCode::FutureWait:
    case vm::OpCode::FutureBlock:
    case vm::OpCode::Dup:
    case vm::OpCode::Pop:
      result.push_back(Instruction(opCode, offset, {}));
      continue;
    case vm::OpCode::LoadLitString:
    case vm::OpCode::LoadLitIp:
    case vm::OpCode::Jump:
    case vm::OpCode::JumpIf:
      result.push_back(Instruction(opCode, offset, {readAsm<uint32_t>(&ip)}));
      continue;
    case vm::OpCode::Call:
    case vm::OpCode::CallTail:
    case vm::OpCode::CallForked:
      result.push_back(
          Instruction(opCode, offset, {readAsm<uint8_t>(&ip), readAsm<uint32_t>(&ip)}));
      continue;
    case vm::OpCode::PCall:
      result.push_back(Instruction(opCode, offset, {readAsm<vm::PCallCode>(&ip)}));
      continue;
    }
    throw std::logic_error{"Bad assembly"};
  }
  return result;
}

} // namespace backend::dasm
