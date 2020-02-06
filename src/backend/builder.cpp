#include "backend/builder.hpp"
#include <limits>
#include <sstream>
#include <stdexcept>

namespace backend {

Builder::Builder() : m_closed{false}, m_genLabelCounter{0U} {}

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
  if (val == 0) {
    writeOpCode(vm::OpCode::LoadLitInt0);
  } else if (val == 1) {
    writeOpCode(vm::OpCode::LoadLitInt1);
  } else if (val >= 0 && val <= std::numeric_limits<uint8_t>::max()) {
    writeOpCode(vm::OpCode::LoadLitIntSmall);
    writeUInt8(static_cast<uint8_t>(val));
  } else {
    writeOpCode(vm::OpCode::LoadLitInt);
    writeInt32(val);
  }
}

auto Builder::addLoadLitFloat(float val) -> void {
  writeOpCode(vm::OpCode::LoadLitFloat);
  writeFloat(val);
}

auto Builder::addLoadLitString(const std::string& val) -> void {
  const auto litId = addLitString(val);
  writeOpCode(vm::OpCode::LoadLitString);
  writeInt32(litId);
}

auto Builder::addLoadLitIp(std::string label) -> void {
  writeOpCode(vm::OpCode::LoadLitIp);
  writeIpOffset(std::move(label));
}

auto Builder::addStackAlloc(uint8_t amount) -> void {
  writeOpCode(vm::OpCode::StackAlloc);
  writeUInt8(amount);
}

auto Builder::addStackStore(uint8_t constId) -> void {
  writeOpCode(vm::OpCode::StackStore);
  writeUInt8(constId);
}

auto Builder::addStackLoad(uint8_t constId) -> void {
  writeOpCode(vm::OpCode::StackLoad);
  writeUInt8(constId);
}

auto Builder::addAddInt() -> void { writeOpCode(vm::OpCode::AddInt); }

auto Builder::addAddFloat() -> void { writeOpCode(vm::OpCode::AddFloat); }

auto Builder::addAddString() -> void { writeOpCode(vm::OpCode::AddString); }

auto Builder::addSubInt() -> void { writeOpCode(vm::OpCode::SubInt); }

auto Builder::addSubFloat() -> void { writeOpCode(vm::OpCode::SubFloat); }

auto Builder::addMulInt() -> void { writeOpCode(vm::OpCode::MulInt); }

auto Builder::addMulFloat() -> void { writeOpCode(vm::OpCode::MulFloat); }

auto Builder::addDivInt() -> void { writeOpCode(vm::OpCode::DivInt); }

auto Builder::addDivFloat() -> void { writeOpCode(vm::OpCode::DivFloat); }

auto Builder::addRemInt() -> void { writeOpCode(vm::OpCode::RemInt); }

auto Builder::addModFloat() -> void { writeOpCode(vm::OpCode::ModFloat); }

auto Builder::addPowFloat() -> void { writeOpCode(vm::OpCode::PowFloat); }

auto Builder::addSqrtFloat() -> void { writeOpCode(vm::OpCode::SqrtFloat); }

auto Builder::addSinFloat() -> void { writeOpCode(vm::OpCode::SinFloat); }

auto Builder::addCosFloat() -> void { writeOpCode(vm::OpCode::CosFloat); }

auto Builder::addTanFloat() -> void { writeOpCode(vm::OpCode::TanFloat); }

auto Builder::addASinFloat() -> void { writeOpCode(vm::OpCode::ASinFloat); }

auto Builder::addACosFloat() -> void { writeOpCode(vm::OpCode::ACosFloat); }

auto Builder::addATanFloat() -> void { writeOpCode(vm::OpCode::ATanFloat); }

auto Builder::addATan2Float() -> void { writeOpCode(vm::OpCode::ATan2Float); }

auto Builder::addNegInt() -> void { writeOpCode(vm::OpCode::NegInt); }

auto Builder::addNegFloat() -> void { writeOpCode(vm::OpCode::NegFloat); }

auto Builder::addLogicInvInt() -> void { writeOpCode(vm::OpCode::LogicInvInt); }

auto Builder::addShiftLeftInt() -> void { writeOpCode(vm::OpCode::ShiftLeftInt); }

auto Builder::addShiftRightInt() -> void { writeOpCode(vm::OpCode::ShiftRightInt); }

auto Builder::addAndInt() -> void { writeOpCode(vm::OpCode::AndInt); }

auto Builder::addOrInt() -> void { writeOpCode(vm::OpCode::OrInt); }

auto Builder::addXorInt() -> void { writeOpCode(vm::OpCode::XorInt); }

auto Builder::addInvInt() -> void { writeOpCode(vm::OpCode::InvInt); }

auto Builder::addLengthString() -> void { writeOpCode(vm::OpCode::LengthString); }

auto Builder::addIndexString() -> void { writeOpCode(vm::OpCode::IndexString); }

auto Builder::addSliceString() -> void { writeOpCode(vm::OpCode::SliceString); }

auto Builder::addCheckEqInt() -> void { writeOpCode(vm::OpCode::CheckEqInt); }

auto Builder::addCheckEqFloat() -> void { writeOpCode(vm::OpCode::CheckEqFloat); }

auto Builder::addCheckEqString() -> void { writeOpCode(vm::OpCode::CheckEqString); }

auto Builder::addCheckEqIp() -> void { writeOpCode(vm::OpCode::CheckEqIp); }

auto Builder::addCheckEqCallDynTgt() -> void { writeOpCode(vm::OpCode::CheckEqCallDynTgt); }

auto Builder::addCheckGtInt() -> void { writeOpCode(vm::OpCode::CheckGtInt); }

auto Builder::addCheckGtFloat() -> void { writeOpCode(vm::OpCode::CheckGtFloat); }

auto Builder::addCheckLeInt() -> void { writeOpCode(vm::OpCode::CheckLeInt); }

auto Builder::addCheckLeFloat() -> void { writeOpCode(vm::OpCode::CheckLeFloat); }

auto Builder::addConvIntFloat() -> void { writeOpCode(vm::OpCode::ConvIntFloat); }

auto Builder::addConvFloatInt() -> void { writeOpCode(vm::OpCode::ConvFloatInt); }

auto Builder::addConvIntString() -> void { writeOpCode(vm::OpCode::ConvIntString); }

auto Builder::addConvFloatString() -> void { writeOpCode(vm::OpCode::ConvFloatString); }

auto Builder::addConvBoolString() -> void {
  // Implemented on the backend to keep the vm simpler.
  const auto endLabel  = generateLabel();
  const auto trueLabel = generateLabel();
  addJumpIf(trueLabel);

  addLoadLitString("false");
  addJump(endLabel);

  label(trueLabel);
  addLoadLitString("true");

  label(endLabel);
}

auto Builder::addConvCharString() -> void { writeOpCode(vm::OpCode::ConvCharString); }

auto Builder::addConvIntChar() -> void { writeOpCode(vm::OpCode::ConvIntChar); }

auto Builder::addMakeStruct(uint8_t fieldCount) -> void {
  writeOpCode(vm::OpCode::MakeStruct);
  writeUInt8(fieldCount);
}

auto Builder::addLoadStructField(uint8_t fieldIndex) -> void {
  writeOpCode(vm::OpCode::LoadStructField);
  writeUInt8(fieldIndex);
}

auto Builder::addJump(std::string label) -> void {
  writeOpCode(vm::OpCode::Jump);
  writeIpOffset(std::move(label));
}

auto Builder::addJumpIf(std::string label) -> void {
  writeOpCode(vm::OpCode::JumpIf);
  writeIpOffset(std::move(label));
}

auto Builder::addCall(std::string label, uint8_t argCount, bool tail) -> void {
  writeOpCode(tail ? vm::OpCode::CallTail : vm::OpCode::Call);
  writeUInt8(argCount);
  writeIpOffset(std::move(label));
}

auto Builder::addCallDyn(uint8_t argCount, bool tail) -> void {
  writeOpCode(tail ? vm::OpCode::CallDynTail : vm::OpCode::CallDyn);
  writeUInt8(argCount);
}

auto Builder::addPCall(vm::PCallCode code) -> void {
  writeOpCode(vm::OpCode::PCall);
  writeUInt8(static_cast<uint8_t>(code));
}

auto Builder::addRet() -> void { writeOpCode(vm::OpCode::Ret); }

auto Builder::addDup() -> void { writeOpCode(vm::OpCode::Dup); }

auto Builder::addPop() -> void { writeOpCode(vm::OpCode::Pop); }

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

  return vm::Assembly{std::move(m_litStrings), std::move(m_instructions), std::move(entryPoints)};
}

auto Builder::addLitString(const std::string& string) -> uint32_t {
  const auto itr = m_litStringLookup.find(string);
  if (itr != m_litStringLookup.end()) {
    return itr->second;
  }
  const auto id = m_litStrings.size();
  m_litStrings.push_back(string);
  m_litStringLookup.insert({string, id});
  return id;
}

auto Builder::getCurrentIpOffset() -> uint32_t { return m_instructions.size(); }

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
  writeUInt32(reinterpret_cast<uint32_t&>(val)); // NOLINT: Reinterpret cast
}

auto Builder::writeUInt32(uint32_t val) -> void {
  throwIfClosed();
  m_instructions.push_back(val);
  m_instructions.push_back(val >> 8U);  // NOLINT: Magic number
  m_instructions.push_back(val >> 16U); // NOLINT: Magic number
  m_instructions.push_back(val >> 24U); // NOLINT: Magic number
}

auto Builder::writeFloat(float val) -> void {
  throwIfClosed();
  writeUInt32(reinterpret_cast<uint32_t&>(val)); // NOLINT: Reinterpret cast
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
