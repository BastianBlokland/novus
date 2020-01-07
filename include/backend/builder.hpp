#pragma once
#include "vm/assembly.hpp"
#include "vm/opcode.hpp"
#include <string>
#include <unordered_map>
#include <utility>

namespace backend {

class Builder final {
public:
  Builder();
  Builder(const Builder& rhs)     = delete;
  Builder(Builder&& rhs) noexcept = default;
  ~Builder()                      = default;

  auto operator=(const Builder& rhs) -> Builder& = delete;
  auto operator=(Builder&& rhs) noexcept -> Builder& = delete;

  auto generateLabel() -> std::string;

  auto label(std::string label) -> void;

  auto addLoadLitInt(int32_t val) -> void;
  auto addLoadLitFloat(float val) -> void;
  auto addLoadLitString(const std::string& val) -> void;
  auto addLoadLitIp(std::string label) -> void;

  auto addReserveConsts(uint8_t amount) -> void;
  auto addStoreConst(uint8_t constId) -> void;
  auto addLoadConst(uint8_t constId) -> void;

  auto addAddInt() -> void;
  auto addAddFloat() -> void;
  auto addAddString() -> void;
  auto addSubInt() -> void;
  auto addSubFloat() -> void;
  auto addMulInt() -> void;
  auto addMulFloat() -> void;
  auto addDivInt() -> void;
  auto addDivFloat() -> void;
  auto addRemInt() -> void;
  auto addNegInt() -> void;
  auto addNegFloat() -> void;
  auto addLogicInvInt() -> void;
  auto addShiftLeftInt() -> void;
  auto addShiftRightInt() -> void;
  auto addAndInt() -> void;
  auto addOrInt() -> void;
  auto addXorInt() -> void;
  auto addLengthString() -> void;

  auto addCheckEqInt() -> void;
  auto addCheckEqFloat() -> void;
  auto addCheckEqString() -> void;
  auto addCheckEqIp() -> void;
  auto addCheckEqCallDynTgt() -> void;
  auto addCheckGtInt() -> void;
  auto addCheckGtFloat() -> void;
  auto addCheckLeInt() -> void;
  auto addCheckLeFloat() -> void;

  auto addConvIntFloat() -> void;
  auto addConvFloatInt() -> void;
  auto addConvIntString() -> void;
  auto addConvFloatString() -> void;
  auto addConvBoolString() -> void;

  auto addMakeStruct(uint8_t fieldCount) -> void;
  auto addLoadStructField(uint8_t fieldIndex) -> void;

  auto addPrintString() -> void;

  auto addJump(std::string label) -> void;
  auto addJumpIf(std::string label) -> void;

  auto addCall(std::string label, bool tail) -> void;
  auto addCallDyn(bool tail) -> void;
  auto addRet() -> void;

  auto addDup() -> void;
  auto addPop() -> void;
  auto addFail() -> void;

  auto addEntryPoint(std::string label) -> void;

  auto close() -> vm::Assembly;

private:
  bool m_closed;
  unsigned int m_genLabelCounter;
  std::unordered_map<std::string, uint32_t> m_litStringLookup;
  std::vector<std::string> m_litStrings;
  std::vector<uint8_t> m_instructions;
  std::vector<std::string> m_entryPointLabels;
  std::unordered_map<std::string, uint32_t> m_labels;
  std::vector<std::pair<std::string, unsigned int>> m_labelTargets;

  [[nodiscard]] auto addLitString(const std::string& string) -> uint32_t;
  [[nodiscard]] auto getCurrentIpOffset() -> uint32_t;

  auto writeOpCode(vm::OpCode opCode) -> void;
  auto writeUInt8(uint8_t val) -> void;
  auto writeInt32(int32_t val) -> void;
  auto writeUInt32(uint32_t val) -> void;
  auto writeFloat(float val) -> void;
  auto writeIpOffset(std::string label) -> void;

  auto patchLabels() -> void;
  auto throwIfClosed() -> void;
};

} // namespace backend
