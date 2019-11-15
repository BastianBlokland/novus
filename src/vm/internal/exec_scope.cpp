#include "internal/exec_scope.hpp"
#include "vm/assembly.hpp"

namespace vm::internal {

ExecScope::ExecScope(const Assembly& assembly, uint32_t ipOffset) :
    m_assembly{assembly}, m_ip{assembly.getIp(ipOffset)} {}

auto ExecScope::readOpCode() -> OpCode { return static_cast<OpCode>(readUInt8()); }

auto ExecScope::readUInt8() -> uint8_t { return *m_ip++; }

auto ExecScope::readInt32() -> int32_t {
  // TODO(bastian): Handle endianess differences.
  int32_t value = *reinterpret_cast<const int32_t*>(m_ip); // NOLINT: Reinterpret cast
  m_ip += 4;
  return value;
}

auto ExecScope::readUInt32() -> uint32_t {
  // TODO(bastian): Handle endianess differences.
  uint32_t value = *reinterpret_cast<const uint32_t*>(m_ip); // NOLINT: Reinterpret cast
  m_ip += 4;
  return value;
}

auto ExecScope::getConst(uint8_t id) -> Value {
  auto itr = m_consts.find(id);
  assert(itr != m_consts.end());
  return itr->second;
}

auto ExecScope::setConst(uint8_t id, Value value) -> void {
  assert(m_consts.find(id) == m_consts.end());
  m_consts.insert({id, value});
}

auto ExecScope::jump(uint32_t ipOffset) -> void { m_ip = m_assembly.getIp(ipOffset); }

} // namespace vm::internal
