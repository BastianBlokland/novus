#include "backend/builder.hpp"
#include <limits>
#include <sstream>
#include <stdexcept>

namespace backend {

Builder::Builder() : m_closed{false}, m_genLabelCounter{0U} {}

auto Builder::getCurrentIpOffset() -> uint32_t { return m_instructions.size(); }

auto Builder::generateLabel() -> std::string {
  std::ostringstream oss;
  oss << "__gen_" << m_genLabelCounter++;
  return oss.str();
}

auto Builder::label(std::string label) -> void {
  throwIfClosed();
  if (!m_labels.insert({std::move(label), getCurrentIpOffset()}).second) {
    throw std::invalid_argument{"Builder already contains a label with the same name"};
  }
}

auto Builder::addLoadLitInt(int32_t val) -> void {
  writeOpCode(vm::OpCode::LoadLitInt);
  writeInt32(val);
}

auto Builder::addReserveConsts(uint8_t amount) -> void {
  writeOpCode(vm::OpCode::ReserveConsts);
  writeUInt8(amount);
}

auto Builder::addStoreConst(uint8_t constId) -> void {
  writeOpCode(vm::OpCode::StoreConst);
  writeUInt8(constId);
}

auto Builder::addLoadConst(uint8_t constId) -> void {
  writeOpCode(vm::OpCode::LoadConst);
  writeUInt8(constId);
}

auto Builder::addAddInt() -> void { writeOpCode(vm::OpCode::AddInt); }

auto Builder::addSubInt() -> void { writeOpCode(vm::OpCode::SubInt); }

auto Builder::addMulInt() -> void { writeOpCode(vm::OpCode::MulInt); }

auto Builder::addDivInt() -> void { writeOpCode(vm::OpCode::DivInt); }

auto Builder::addNegInt() -> void { writeOpCode(vm::OpCode::NegInt); }

auto Builder::addLogicInvInt() -> void { writeOpCode(vm::OpCode::LogicInvInt); }

auto Builder::addCheckEqInt() -> void { writeOpCode(vm::OpCode::CheckEqInt); }

auto Builder::addCheckGtInt() -> void { writeOpCode(vm::OpCode::CheckGtInt); }

auto Builder::addCheckLeInt() -> void { writeOpCode(vm::OpCode::CheckLeInt); }

auto Builder::addPrintInt() -> void { writeOpCode(vm::OpCode::PrintInt); }

auto Builder::addPrintLogic() -> void { writeOpCode(vm::OpCode::PrintLogic); }

auto Builder::addJump(std::string label) -> void {
  writeOpCode(vm::OpCode::Jump);
  writeIpOffset(std::move(label));
}

auto Builder::addJumpIf(std::string label) -> void {
  writeOpCode(vm::OpCode::JumpIf);
  writeIpOffset(std::move(label));
}

auto Builder::addCall(std::string label) -> void {
  writeOpCode(vm::OpCode::Call);
  writeIpOffset(std::move(label));
}

auto Builder::addRet() -> void { writeOpCode(vm::OpCode::Ret); }

auto Builder::addFail() -> void { writeOpCode(vm::OpCode::Fail); }

auto Builder::addEntryPoint(std::string label) -> void {
  m_entryPointLabels.push_back(std::move(label));
}

auto Builder::close() -> vm::Assembly {
  throwIfClosed();

  m_closed = true;
  patchLabels();

  auto entryPoints = std::vector<uint32_t>{};
  entryPoints.reserve(m_entryPointLabels.size());
  for (const auto& entryPointLabel : m_entryPointLabels) {
    auto ipOffsetItr = m_labels.find(entryPointLabel);
    if (ipOffsetItr == m_labels.end()) {
      std::ostringstream oss;
      oss << "No declaration for label '" << entryPointLabel << "' found";
      throw std::logic_error{oss.str()};
    }
    entryPoints.push_back(ipOffsetItr->second);
  }

  return vm::Assembly{std::move(m_instructions), std::move(entryPoints)};
}

auto Builder::writeOpCode(vm::OpCode opCode) -> void {
  throwIfClosed();
  writeUInt8(static_cast<uint8_t>(opCode));
}

auto Builder::writeUInt8(uint8_t val) -> void {
  throwIfClosed();
  m_instructions.push_back(val);
}

auto Builder::writeInt32(int32_t val) -> void {
  throwIfClosed();
  writeUInt32(static_cast<uint32_t>(val));
}

auto Builder::writeUInt32(uint32_t val) -> void {
  throwIfClosed();
  m_instructions.push_back(val);
  m_instructions.push_back(val >> 8U);  // NOLINT: Magic number
  m_instructions.push_back(val >> 16U); // NOLINT: Magic number
  m_instructions.push_back(val >> 24U); // NOLINT: Magic number
}

auto Builder::writeIpOffset(std::string label) -> void {
  throwIfClosed();
  // To allow writing a ipOffset before we've written those instructions we write a dummy offset and
  // save the location so we can patch this later with the real ipOffset.
  m_labelTargets.emplace_back(std::move(label), m_instructions.size());
  m_instructions.insert(m_instructions.end(), 4, std::numeric_limits<uint8_t>::max());
}

auto Builder::patchLabels() -> void {
  for (const auto& target : m_labelTargets) {
    auto ipOffsetItr = m_labels.find(target.first);
    if (ipOffsetItr == m_labels.end()) {
      std::ostringstream oss;
      oss << "No declaration for label '" << target.first << "' found";
      throw std::logic_error{oss.str()};
    }
    auto ipOffset                     = ipOffsetItr->second;
    m_instructions[target.second]     = ipOffset;
    m_instructions[target.second + 1] = ipOffset >> 8U;  // NOLINT: Magic number
    m_instructions[target.second + 2] = ipOffset >> 16U; // NOLINT: Magic number
    m_instructions[target.second + 3] = ipOffset >> 24U; // NOLINT: Magic number
  }
}

auto Builder::throwIfClosed() -> void {
  if (m_closed) {
    throw std::logic_error{"Builder is closed"};
  }
}

} // namespace backend
