#pragma once
#include "internal/const_stack.hpp"
#include "internal/value.hpp"
#include "vm/assembly.hpp"
#include "vm/opcode.hpp"
#include "vm/pcall_code.hpp"
#include <cstdint>

namespace vm::internal {

class CallStack;

class ExecScope final {
  friend CallStack;

public:
  ExecScope()                         = default;
  ExecScope(const ExecScope& rhs)     = delete;
  ExecScope(ExecScope&& rhs) noexcept = delete;
  ~ExecScope()                        = default;

  auto operator=(const ExecScope& rhs) -> ExecScope& = delete;
  auto operator=(ExecScope&& rhs) noexcept -> ExecScope& = delete;

  [[nodiscard]] auto readOpCode() -> OpCode;
  [[nodiscard]] auto readPCallCode() -> PCallCode;
  [[nodiscard]] auto readUInt8() -> uint8_t;
  [[nodiscard]] auto readInt32() -> int32_t;
  [[nodiscard]] auto readUInt32() -> uint32_t;
  [[nodiscard]] auto readFloat() -> float;

  auto reserveConsts(ConstStack* stack, unsigned int amount) -> void;
  auto releaseConsts(ConstStack* stack) -> void;

  auto getConst(uint8_t id) -> Value;
  auto setConst(uint8_t id, Value value) -> void;

  inline auto jump(const uint8_t* ip) -> void { m_ip = ip; }

private:
  const uint8_t* m_ip;
  Value* m_constsPtr;
  unsigned int m_constsCount;
};

} // namespace vm::internal
