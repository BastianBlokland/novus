#include "backend/dasm/disassembler.hpp"
#include "backend/dasm/instruction.hpp"
#include <stdexcept>

namespace backend::dasm {

auto disassembleInstructions(const vm::Assembly& assembly) -> std::vector<Instruction> {
  auto result = std::vector<Instruction>{};

  const auto& instrData = assembly.getInstructions();
  for (auto ipOffset = 0U; ipOffset < instrData.size(); ++ipOffset) {
    auto opCode = static_cast<vm::OpCode>(instrData[ipOffset]);
    switch (opCode) {
    case vm::OpCode::LoadLitInt:
      result.push_back(Instruction(opCode, ipOffset, {assembly.readInt32(ipOffset + 1)}));
      ipOffset += 4;
      continue;
    case vm::OpCode::LoadLitFloat:
      result.push_back(Instruction(opCode, ipOffset, {assembly.readFloat(ipOffset + 1)}));
      ipOffset += 4;
      continue;
    case vm::OpCode::LoadLitIntSmall:
    case vm::OpCode::ReserveConsts:
    case vm::OpCode::LoadConst:
    case vm::OpCode::StoreConst:
    case vm::OpCode::MakeStruct:
    case vm::OpCode::LoadStructField:
      result.push_back(Instruction(opCode, ipOffset, {instrData[ipOffset + 1]}));
      ++ipOffset;
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
    case vm::OpCode::NegInt:
    case vm::OpCode::NegFloat:
    case vm::OpCode::LogicInvInt:
    case vm::OpCode::CheckEqInt:
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
    case vm::OpCode::ConvFloatString:
    case vm::OpCode::PrintString:
    case vm::OpCode::CallDyn:
    case vm::OpCode::Ret:
    case vm::OpCode::Fail:
    case vm::OpCode::Dup:
    case vm::OpCode::Pop:
      result.push_back(Instruction(opCode, ipOffset, {}));
      continue;
    case vm::OpCode::LoadLitString:
    case vm::OpCode::LoadLitIp:
    case vm::OpCode::Jump:
    case vm::OpCode::JumpIf:
    case vm::OpCode::Call:
      result.push_back(Instruction(opCode, ipOffset, {assembly.readUInt32(ipOffset + 1)}));
      ipOffset += 4;
      continue;
    }
    throw std::logic_error{"Bad assembly"};
  }
  return result;
}

} // namespace backend::dasm
