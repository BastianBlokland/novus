#pragma once
#include "internal/const_stack.hpp"
#include "internal/value.hpp"
#include "vm/assembly.hpp"
#include "vm/op_code.hpp"
#include "vm/pcall_code.hpp"
#include "vm/result_code.hpp"
#include <cstdint>

namespace vm::internal {

class CallStack;

class ExecScope final {
  friend CallStack;

public:
  ExecScope() noexcept            = default;
  ExecScope(const ExecScope& rhs) = delete;
  ExecScope(ExecScope&& rhs)      = delete;
  ~ExecScope() noexcept           = default;

  auto operator=(const ExecScope& rhs) -> ExecScope& = delete;
  auto operator=(ExecScope&& rhs) -> ExecScope& = delete;

  [[nodiscard]] auto readOpCode() noexcept -> OpCode;
  [[nodiscard]] auto readPCallCode() noexcept -> PCallCode;
  [[nodiscard]] auto readUInt8() noexcept -> uint8_t;
  [[nodiscard]] auto readInt32() noexcept -> int32_t;
  [[nodiscard]] auto readUInt32() noexcept -> uint32_t;
  [[nodiscard]] auto readFloat() noexcept -> float;

  [[nodiscard]] auto reserveConsts(ConstStack* stack, unsigned int amount) noexcept -> ResultCode;
  auto releaseConsts(ConstStack* stack) noexcept -> void;

  auto getConst(uint8_t id) noexcept -> Value;
  auto setConst(uint8_t id, Value value) noexcept -> void;

  inline auto jump(const uint8_t* ip) noexcept -> void { m_ip = ip; }

private:
  const uint8_t* m_ip;
  Value* m_constsPtr;
  unsigned int m_constsCount;
};

} // namespace vm::internal
