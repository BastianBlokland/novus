#include "novasm/assembler.hpp"
#include <limits>
#include <sstream>
#include <stdexcept>

namespace novasm {

Assembler::Assembler() : m_closed{false}, m_genLabelCounter{0U} {}

auto Assembler::generateLabel(const std::string& prefix) -> std::string {
  std::ostringstream oss;
  oss << prefix << "_" << m_genLabelCounter++;
  return oss.str();
}

auto Assembler::label(std::string label) -> void {
  throwIfClosed();
  if (!m_labels.insert({std::move(label), getCurrentIpOffset()}).second) {
    throw std::invalid_argument{"Assembler already contains a label with the same name"};
  }
}

auto Assembler::addLoadLitInt(int32_t val) -> void {
  if (val == 0) {
    writeOpCode(OpCode::LoadLitInt0);
  } else if (val == 1) {
    writeOpCode(OpCode::LoadLitInt1);
  } else if (val >= 0 && val <= std::numeric_limits<uint8_t>::max()) {
    writeOpCode(OpCode::LoadLitIntSmall);
    writeUInt8(static_cast<uint8_t>(val));
  } else {
    writeOpCode(OpCode::LoadLitInt);
    writeInt32(val);
  }
}

auto Assembler::addLoadLitLong(int64_t val) -> void {
  writeOpCode(OpCode::LoadLitLong);
  writeInt64(val);
}

auto Assembler::addLoadLitFloat(float val) -> void {
  writeOpCode(OpCode::LoadLitFloat);
  writeFloat(val);
}

auto Assembler::addLoadLitString(const std::string& val) -> void {
  const auto litId = addLitString(val);
  writeOpCode(OpCode::LoadLitString);
  writeUInt32(litId);
}

auto Assembler::addLoadLitIp(std::string label) -> void {
  writeOpCode(OpCode::LoadLitIp);
  writeIpOffset(std::move(label));
}

auto Assembler::addStackAlloc(uint8_t amount) -> void {
  writeOpCode(OpCode::StackAlloc);
  writeUInt8(amount);
}

auto Assembler::addStackStore(uint8_t offset) -> void {
  writeOpCode(OpCode::StackStore);
  writeUInt8(offset);
}

auto Assembler::addStackLoad(uint8_t offset) -> void {
  writeOpCode(OpCode::StackLoad);
  writeUInt8(offset);
}

auto Assembler::addAddInt() -> void { writeOpCode(OpCode::AddInt); }

auto Assembler::addAddLong() -> void { writeOpCode(OpCode::AddLong); }

auto Assembler::addAddFloat() -> void { writeOpCode(OpCode::AddFloat); }

auto Assembler::addAddString() -> void { writeOpCode(OpCode::AddString); }

auto Assembler::addAddChar() -> void { writeOpCode(OpCode::AddChar); }

auto Assembler::addSubInt() -> void { writeOpCode(OpCode::SubInt); }

auto Assembler::addSubLong() -> void { writeOpCode(OpCode::SubLong); }

auto Assembler::addSubFloat() -> void { writeOpCode(OpCode::SubFloat); }

auto Assembler::addMulInt() -> void { writeOpCode(OpCode::MulInt); }

auto Assembler::addMulLong() -> void { writeOpCode(OpCode::MulLong); }

auto Assembler::addMulFloat() -> void { writeOpCode(OpCode::MulFloat); }

auto Assembler::addDivInt() -> void { writeOpCode(OpCode::DivInt); }

auto Assembler::addDivLong() -> void { writeOpCode(OpCode::DivLong); }

auto Assembler::addDivFloat() -> void { writeOpCode(OpCode::DivFloat); }

auto Assembler::addRemInt() -> void { writeOpCode(OpCode::RemInt); }

auto Assembler::addRemLong() -> void { writeOpCode(OpCode::RemLong); }

auto Assembler::addModFloat() -> void { writeOpCode(OpCode::ModFloat); }

auto Assembler::addPowFloat() -> void { writeOpCode(OpCode::PowFloat); }

auto Assembler::addSqrtFloat() -> void { writeOpCode(OpCode::SqrtFloat); }

auto Assembler::addSinFloat() -> void { writeOpCode(OpCode::SinFloat); }

auto Assembler::addCosFloat() -> void { writeOpCode(OpCode::CosFloat); }

auto Assembler::addTanFloat() -> void { writeOpCode(OpCode::TanFloat); }

auto Assembler::addASinFloat() -> void { writeOpCode(OpCode::ASinFloat); }

auto Assembler::addACosFloat() -> void { writeOpCode(OpCode::ACosFloat); }

auto Assembler::addATanFloat() -> void { writeOpCode(OpCode::ATanFloat); }

auto Assembler::addATan2Float() -> void { writeOpCode(OpCode::ATan2Float); }

auto Assembler::addNegInt() -> void { writeOpCode(OpCode::NegInt); }

auto Assembler::addNegLong() -> void { writeOpCode(OpCode::NegLong); }

auto Assembler::addNegFloat() -> void { writeOpCode(OpCode::NegFloat); }

auto Assembler::addLogicInvInt() -> void { writeOpCode(OpCode::LogicInvInt); }

auto Assembler::addShiftLeftInt() -> void { writeOpCode(OpCode::ShiftLeftInt); }

auto Assembler::addShiftRightInt() -> void { writeOpCode(OpCode::ShiftRightInt); }

auto Assembler::addAndInt() -> void { writeOpCode(OpCode::AndInt); }

auto Assembler::addOrInt() -> void { writeOpCode(OpCode::OrInt); }

auto Assembler::addXorInt() -> void { writeOpCode(OpCode::XorInt); }

auto Assembler::addInvInt() -> void { writeOpCode(OpCode::InvInt); }

auto Assembler::addLengthString() -> void { writeOpCode(OpCode::LengthString); }

auto Assembler::addIndexString() -> void { writeOpCode(OpCode::IndexString); }

auto Assembler::addSliceString() -> void { writeOpCode(OpCode::SliceString); }

auto Assembler::addCheckEqInt() -> void { writeOpCode(OpCode::CheckEqInt); }

auto Assembler::addCheckEqLong() -> void { writeOpCode(OpCode::CheckEqLong); }

auto Assembler::addCheckEqFloat() -> void { writeOpCode(OpCode::CheckEqFloat); }

auto Assembler::addCheckEqString() -> void { writeOpCode(OpCode::CheckEqString); }

auto Assembler::addCheckEqIp() -> void { writeOpCode(OpCode::CheckEqIp); }

auto Assembler::addCheckEqCallDynTgt() -> void { writeOpCode(OpCode::CheckEqCallDynTgt); }

auto Assembler::addCheckGtInt() -> void { writeOpCode(OpCode::CheckGtInt); }

auto Assembler::addCheckGtLong() -> void { writeOpCode(OpCode::CheckGtLong); }

auto Assembler::addCheckGtFloat() -> void { writeOpCode(OpCode::CheckGtFloat); }

auto Assembler::addCheckLeInt() -> void { writeOpCode(OpCode::CheckLeInt); }

auto Assembler::addCheckLeLong() -> void { writeOpCode(OpCode::CheckLeLong); }

auto Assembler::addCheckLeFloat() -> void { writeOpCode(OpCode::CheckLeFloat); }

auto Assembler::addConvIntLong() -> void { writeOpCode(OpCode::ConvIntLong); }

auto Assembler::addConvIntFloat() -> void { writeOpCode(OpCode::ConvIntFloat); }

auto Assembler::addConvLongInt() -> void { writeOpCode(OpCode::ConvLongInt); }

auto Assembler::addConvFloatInt() -> void { writeOpCode(OpCode::ConvFloatInt); }

auto Assembler::addConvIntString() -> void { writeOpCode(OpCode::ConvIntString); }

auto Assembler::addConvLongString() -> void { writeOpCode(OpCode::ConvLongString); }

auto Assembler::addConvFloatString() -> void { writeOpCode(OpCode::ConvFloatString); }

auto Assembler::addConvBoolString() -> void {
  // Implemented on the backend to keep the vm simpler.
  const auto endLabel  = generateLabel("bool-to-string-end");
  const auto trueLabel = generateLabel("bool-to-string-true");
  addJumpIf(trueLabel);

  addLoadLitString("false");
  addJump(endLabel);

  label(trueLabel);
  addLoadLitString("true");

  label(endLabel);
}

auto Assembler::addConvCharString() -> void { writeOpCode(OpCode::ConvCharString); }

auto Assembler::addConvIntChar() -> void { writeOpCode(OpCode::ConvIntChar); }

auto Assembler::addConvFloatChar() -> void { writeOpCode(OpCode::ConvFloatChar); }

auto Assembler::addMakeStruct(uint8_t fieldCount) -> void {
  writeOpCode(OpCode::MakeStruct);
  writeUInt8(fieldCount);
}

auto Assembler::addLoadStructField(uint8_t fieldIndex) -> void {
  writeOpCode(OpCode::LoadStructField);
  writeUInt8(fieldIndex);
}

auto Assembler::addJump(std::string label) -> void {
  writeOpCode(OpCode::Jump);
  writeIpOffset(std::move(label));
}

auto Assembler::addJumpIf(std::string label) -> void {
  writeOpCode(OpCode::JumpIf);
  writeIpOffset(std::move(label));
}

auto Assembler::addCall(std::string label, uint8_t argCount, CallMode mode) -> void {
  switch (mode) {
  case CallMode::Normal:
    writeOpCode(OpCode::Call);
    break;
  case CallMode::Tail:
    writeOpCode(OpCode::CallTail);
    break;
  case CallMode::Forked:
    writeOpCode(OpCode::CallForked);
    break;
  default:
    throw std::invalid_argument{"Unsupported call mode"};
  }
  writeUInt8(argCount);
  writeIpOffset(std::move(label));
}

auto Assembler::addCallDyn(uint8_t argCount, CallMode mode) -> void {
  switch (mode) {
  case CallMode::Normal:
    writeOpCode(OpCode::CallDyn);
    break;
  case CallMode::Tail:
    writeOpCode(OpCode::CallDynTail);
    break;
  case CallMode::Forked:
    writeOpCode(OpCode::CallDynForked);
    break;
  default:
    throw std::invalid_argument{"Unsupported call mode"};
  }
  writeUInt8(argCount);
}

auto Assembler::addPCall(PCallCode code) -> void {
  writeOpCode(OpCode::PCall);
  writeUInt8(static_cast<uint8_t>(code));
}

auto Assembler::addRet() -> void { writeOpCode(OpCode::Ret); }

auto Assembler::addFutureWaitNano() -> void { writeOpCode(OpCode::FutureWaitNano); }

auto Assembler::addFutureBlock() -> void { writeOpCode(OpCode::FutureBlock); }

auto Assembler::addDup() -> void { writeOpCode(OpCode::Dup); }

auto Assembler::addPop() -> void { writeOpCode(OpCode::Pop); }

auto Assembler::addFail() -> void { writeOpCode(OpCode::Fail); }

auto Assembler::setEntrypoint(std::string label) -> void { m_entrypointLabel = std::move(label); }

auto Assembler::close() -> Assembly {
  throwIfClosed();

  m_closed = true;
  patchLabels();

  auto entrypointItr = m_labels.find(m_entrypointLabel);
  if (entrypointItr == m_labels.end()) {
    std::ostringstream oss;
    oss << "No declaration for entrypoint '" << m_entrypointLabel << "' found";
    throw std::logic_error{oss.str()};
  }

  return Assembly{entrypointItr->second, std::move(m_litStrings), std::move(m_instructions)};
}

auto Assembler::getLabels() -> std::unordered_map<uint32_t, std::vector<std::string>> {
  auto result = std::unordered_map<uint32_t, std::vector<std::string>>{};
  for (const auto& label : m_labels) {
    const auto resultItr = result.insert({label.second, std::vector<std::string>{}});
    resultItr.first->second.push_back(label.first);
  }
  return result;
}

auto Assembler::addLitString(const std::string& string) -> uint32_t {
  const auto itr = m_litStringLookup.find(string);
  if (itr != m_litStringLookup.end()) {
    return itr->second;
  }
  const auto id = m_litStrings.size();
  m_litStrings.push_back(string);
  m_litStringLookup.insert({string, id});
  return id;
}

auto Assembler::getCurrentIpOffset() -> uint32_t { return m_instructions.size(); }

auto Assembler::writeOpCode(OpCode opCode) -> void { writeUInt8(static_cast<uint8_t>(opCode)); }

auto Assembler::writeUInt8(uint8_t val) -> void {
  throwIfClosed();
  m_instructions.push_back(val);
}

auto Assembler::writeInt32(int32_t val) -> void {
  writeUInt32(reinterpret_cast<uint32_t&>(val)); // NOLINT: Reinterpret cast
}

auto Assembler::writeInt64(int64_t val) -> void {
  writeUInt64(reinterpret_cast<uint64_t&>(val)); // NOLINT: Reinterpret cast
}

auto Assembler::writeUInt32(uint32_t val) -> void {
  throwIfClosed();
  m_instructions.push_back(val);
  m_instructions.push_back(val >> 8U);  // NOLINT: Magic number
  m_instructions.push_back(val >> 16U); // NOLINT: Magic number
  m_instructions.push_back(val >> 24U); // NOLINT: Magic number
}

auto Assembler::writeUInt64(uint64_t val) -> void {
  throwIfClosed();
  m_instructions.push_back(val);
  m_instructions.push_back(val >> 8U);  // NOLINT: Magic number
  m_instructions.push_back(val >> 16U); // NOLINT: Magic number
  m_instructions.push_back(val >> 24U); // NOLINT: Magic number
  m_instructions.push_back(val >> 32U); // NOLINT: Magic number
  m_instructions.push_back(val >> 40U); // NOLINT: Magic number
  m_instructions.push_back(val >> 48U); // NOLINT: Magic number
  m_instructions.push_back(val >> 56U); // NOLINT: Magic number
}

auto Assembler::writeFloat(float val) -> void {
  writeUInt32(reinterpret_cast<uint32_t&>(val)); // NOLINT: Reinterpret cast
}

auto Assembler::writeIpOffset(std::string label) -> void {
  throwIfClosed();
  // To allow writing a ipOffset before we've written those instructions we write a dummy offset and
  // save the location so we can patch this later with the real ipOffset.
  m_labelTargets.emplace_back(std::move(label), m_instructions.size());
  m_instructions.insert(m_instructions.end(), 4, std::numeric_limits<uint8_t>::max());
}

auto Assembler::patchLabels() -> void {
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

auto Assembler::throwIfClosed() -> void {
  if (m_closed) {
    throw std::logic_error{"Assembler is closed"};
  }
}

} // namespace novasm
