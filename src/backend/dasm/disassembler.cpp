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
    case vm::OpCode::LoadConst:
    case vm::OpCode::StoreConst:
      result.push_back(Instruction(opCode, ipOffset, {instrData[ipOffset + 1]}));
      ++ipOffset;
      continue;
    case vm::OpCode::AddInt:
    case vm::OpCode::SubInt:
    case vm::OpCode::MulInt:
    case vm::OpCode::DivInt:
    case vm::OpCode::NegInt:
    case vm::OpCode::LogicInvInt:
    case vm::OpCode::CheckEqInt:
    case vm::OpCode::CheckGtInt:
    case vm::OpCode::CheckLeInt:
    case vm::OpCode::PrintInt:
    case vm::OpCode::PrintLogic:
    case vm::OpCode::Ret:
    case vm::OpCode::Fail:
      result.push_back(Instruction(opCode, ipOffset, {}));
      continue;
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
