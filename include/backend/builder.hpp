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

  auto addStoreConst(uint8_t constId) -> void;
  auto addLoadConst(uint8_t constId) -> void;

  auto addAddInt() -> void;
  auto addSubInt() -> void;
  auto addMulInt() -> void;
  auto addDivInt() -> void;
  auto addNegInt() -> void;
  auto addLogicInvInt() -> void;

  auto addCheckEqInt() -> void;
  auto addCheckGtInt() -> void;
  auto addCheckLeInt() -> void;

  auto addPrintInt() -> void;
  auto addPrintLogic() -> void;

  auto addJump(std::string label) -> void;
  auto addJumpIf(std::string label) -> void;

  auto addCall(std::string label) -> void;
  auto addRet() -> void;
  auto addFail() -> void;

  auto addEntryPoint(std::string label) -> void;

  auto close() -> vm::Assembly;

private:
  bool m_closed;
  unsigned int m_genLabelCounter;
  std::vector<uint8_t> m_instructions;
  std::vector<std::string> m_entryPointLabels;
  std::unordered_map<std::string, uint32_t> m_labels;
  std::vector<std::pair<std::string, unsigned int>> m_labelTargets;

  [[nodiscard]] auto getCurrentIpOffset() -> uint32_t;

  auto writeOpCode(vm::OpCode opCode) -> void;
  auto writeUInt8(uint8_t val) -> void;
  auto writeInt32(int32_t val) -> void;
  auto writeUInt32(uint32_t val) -> void;
  auto writeIpOffset(std::string label) -> void;

  auto patchLabels() -> void;
  auto throwIfClosed() -> void;
};

} // namespace backend
