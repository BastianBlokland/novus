#pragma once
#include "internal/value.hpp"
#include "vm/opcode.hpp"
#include "vm/program.hpp"
#include <cstdint>
#include <unordered_map>

namespace vm::internal {

class ExecScope final {
public:
  explicit ExecScope(const Program& program, uint32_t ipOffset);

  [[nodiscard]] auto readOpCode() -> OpCode;
  [[nodiscard]] auto readUInt8() -> uint8_t;
  [[nodiscard]] auto readInt32() -> int32_t;
  [[nodiscard]] auto readUInt32() -> uint32_t;

  auto getConst(uint8_t id) -> Value;
  auto setConst(uint8_t id, Value value) -> void;

  auto jump(uint32_t ipOffset) -> void;

private:
  const Program& m_program;
  const uint8_t* m_ip;
  std::unordered_map<uint8_t, Value> m_consts;
};

} // namespace vm::internal
