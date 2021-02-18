#pragma once
#include "novasm/assembly.hpp"
#include "novasm/op_code.hpp"
#include "novasm/pcall_code.hpp"
#include <string>
#include <unordered_map>
#include <utility>

namespace novasm {

// Different ways of how a call can be executed.
enum class CallMode {
  Normal,
  Tail, // Tail calls are executed in the same stack frame (doesn't return to the current function).
  Forked, // Forked calls are ran on a new executor (new thread).
};

// Builder class to aid in generating novus assembly.
class Assembler final {
public:
  Assembler(std::string compilerVersion);
  Assembler(const Assembler& rhs)     = delete;
  Assembler(Assembler&& rhs) noexcept = default;
  ~Assembler()                        = default;

  auto operator=(const Assembler& rhs) -> Assembler& = delete;
  auto operator=(Assembler&& rhs) noexcept -> Assembler& = delete;

  auto generateLabel(const std::string& prefix) -> std::string;

  auto label(std::string label) -> void;

  auto addLoadLitInt(int32_t val) -> void;
  auto addLoadLitLong(int64_t val) -> void;
  auto addLoadLitFloat(float val) -> void;
  auto addLoadLitString(const std::string& val) -> void;
  auto addLoadLitIp(std::string label) -> void;

  auto addStackAlloc(uint16_t amount) -> void;
  auto addStackStore(uint16_t offset) -> void;
  auto addStackLoad(uint16_t offset) -> void;

  auto addAddInt() -> void;
  auto addAddLong() -> void;
  auto addAddFloat() -> void;
  auto addAddString() -> void;
  auto addAppendChar() -> void;
  auto addSubInt() -> void;
  auto addSubLong() -> void;
  auto addSubFloat() -> void;
  auto addMulInt() -> void;
  auto addMulLong() -> void;
  auto addMulFloat() -> void;
  auto addDivInt() -> void;
  auto addDivLong() -> void;
  auto addDivFloat() -> void;
  auto addRemInt() -> void;
  auto addRemLong() -> void;
  auto addModFloat() -> void;
  auto addPowFloat() -> void;
  auto addSqrtFloat() -> void;
  auto addSinFloat() -> void;
  auto addCosFloat() -> void;
  auto addTanFloat() -> void;
  auto addASinFloat() -> void;
  auto addACosFloat() -> void;
  auto addATanFloat() -> void;
  auto addATan2Float() -> void;
  auto addNegInt() -> void;
  auto addNegLong() -> void;
  auto addNegFloat() -> void;
  auto addLogicInvInt() -> void;
  auto addShiftLeftInt() -> void;
  auto addShiftLeftLong() -> void;
  auto addShiftRightInt() -> void;
  auto addShiftRightLong() -> void;
  auto addAndInt() -> void;
  auto addAndLong() -> void;
  auto addOrInt() -> void;
  auto addOrLong() -> void;
  auto addXorInt() -> void;
  auto addXorLong() -> void;
  auto addInvInt() -> void;
  auto addInvLong() -> void;
  auto addLengthString() -> void;
  auto addIndexString() -> void;
  auto addSliceString() -> void;

  auto addCheckEqInt() -> void;
  auto addCheckEqLong() -> void;
  auto addCheckEqFloat() -> void;
  auto addCheckEqString() -> void;
  auto addCheckEqIp() -> void;
  auto addCheckEqCallDynTgt() -> void;
  auto addCheckGtInt() -> void;
  auto addCheckGtLong() -> void;
  auto addCheckGtFloat() -> void;
  auto addCheckLeInt() -> void;
  auto addCheckLeLong() -> void;
  auto addCheckLeFloat() -> void;
  auto addCheckStructNull() -> void;

  auto addConvIntLong() -> void;
  auto addConvIntFloat() -> void;
  auto addConvLongInt() -> void;
  auto addConvLongFloat() -> void;
  auto addConvFloatInt() -> void;
  auto addConvIntString() -> void;
  auto addConvLongString() -> void;
  auto addConvFloatString() -> void;
  auto addConvBoolString() -> void;
  auto addConvCharString() -> void;
  auto addConvIntChar() -> void;
  auto addConvLongChar() -> void;
  auto addConvFloatChar() -> void;
  auto addConvFloatLong() -> void;

  auto addMakeAtomic(int32_t val) -> void;
  auto addAtomicLoad() -> void;
  auto addAtomicCompareSwap(int32_t expected, int32_t desired) -> void;
  auto addAtomicBlock(int32_t expected) -> void;

  auto addMakeStruct(uint8_t fieldCount) -> void;
  auto addMakeNullStruct() -> void;
  auto addStructLoadField(uint8_t fieldIndex) -> void;
  auto addStructStoreField(uint8_t fieldIndex) -> void;

  auto addJump(std::string label) -> void;
  auto addJumpIf(std::string label) -> void;

  auto addCall(std::string label, uint8_t argCount, CallMode mode) -> void;
  auto addCallDyn(uint8_t argCount, CallMode mode) -> void;
  auto addPCall(PCallCode code) -> void;
  auto addRet() -> void;

  auto addFutureWaitNano() -> void;
  auto addFutureBlock() -> void;
  auto addDup() -> void;
  auto addPop() -> void;
  auto addSwap() -> void;
  auto addFail() -> void;

  auto setEntrypoint(std::string label) -> void;

  [[nodiscard]] auto close() -> Assembly;
  [[nodiscard]] auto getLabels() -> std::unordered_map<uint32_t, std::vector<std::string>>;

private:
  std::string m_compilerVersion;

  bool m_closed;
  unsigned int m_genLabelCounter;
  std::unordered_map<std::string, uint32_t> m_litStringLookup;
  std::vector<std::string> m_litStrings;
  std::vector<uint8_t> m_instructions;
  std::string m_entrypointLabel;
  std::unordered_map<std::string, uint32_t> m_labels;
  std::vector<std::pair<std::string, unsigned int>> m_labelTargets;

  [[nodiscard]] auto addLitString(const std::string& string) -> uint32_t;
  [[nodiscard]] auto getCurrentIpOffset() -> uint32_t;

  auto writeOpCode(OpCode opCode) -> void;
  auto writeUInt8(uint8_t val) -> void;
  auto writeUInt16(uint16_t val) -> void;
  auto writeInt32(int32_t val) -> void;
  auto writeInt64(int64_t val) -> void;
  auto writeUInt32(uint32_t val) -> void;
  auto writeUInt64(uint64_t val) -> void;
  auto writeFloat(float val) -> void;
  auto writeIpOffset(std::string label) -> void;

  auto patchLabels() -> void;
  auto throwIfClosed() -> void;
};

} // namespace novasm
