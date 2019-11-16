#pragma once
#include "internal/const_stack.hpp"
#include "internal/value.hpp"
#include "vm/assembly.hpp"
#include "vm/opcode.hpp"
#include <cstdint>

namespace vm::internal {

class ExecScope final {
public:
  explicit ExecScope(const Assembly& assembly, uint32_t ipOffset);

  [[nodiscard]] auto readOpCode() -> OpCode;
  [[nodiscard]] auto readUInt8() -> uint8_t;
  [[nodiscard]] auto readInt32() -> int32_t;
  [[nodiscard]] auto readUInt32() -> uint32_t;

  auto reserveConsts(ConstStack* stack, unsigned int amount) -> void;
  auto releaseConsts(ConstStack* stack) -> void;

  auto getConst(uint8_t id) -> Value;
  auto setConst(uint8_t id, Value value) -> void;

  auto jump(uint32_t ipOffset) -> void;

private:
  const Assembly& m_assembly;
  const uint8_t* m_ip;
  unsigned int m_constsCount;
  Value* m_constsPtr;
};

} // namespace vm::internal
