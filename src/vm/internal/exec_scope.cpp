#include "internal/exec_scope.hpp"
#include "vm/assembly.hpp"

namespace vm::internal {

auto ExecScope::readOpCode() noexcept -> OpCode { return static_cast<OpCode>(readUInt8()); }

auto ExecScope::readPCallCode() noexcept -> PCallCode {
  return static_cast<PCallCode>(readUInt8());
}

auto ExecScope::readUInt8() noexcept -> uint8_t { return *m_ip++; }

auto ExecScope::readInt32() noexcept -> int32_t {
  // TODO(bastian): Handle endianess differences.
  int32_t value = *reinterpret_cast<const int32_t*>(m_ip); // NOLINT: Reinterpret cast
  m_ip += 4;
  return value;
}

auto ExecScope::readUInt32() noexcept -> uint32_t {
  // TODO(bastian): Handle endianess differences.
  uint32_t value = *reinterpret_cast<const uint32_t*>(m_ip); // NOLINT: Reinterpret cast
  m_ip += 4;
  return value;
}

auto ExecScope::readFloat() noexcept -> float {
  // TODO(bastian): Handle endianess differences.
  float value = *reinterpret_cast<const float*>(m_ip); // NOLINT: Reinterpret cast
  m_ip += 4;
  return value;
}

auto ExecScope::reserveConsts(ConstStack* stack, unsigned int amount) noexcept -> ResultCode {
  if (m_constsPtr == nullptr) {
    m_constsCount = amount;
    return stack->reserve(amount, &m_constsPtr);
  }
  if (amount > m_constsCount) {
    const auto toReserve = amount - m_constsCount;
    m_constsCount        = amount;
    Value* unused;
    return stack->reserve(toReserve, &unused);
  }
  return ResultCode::Ok;
}

auto ExecScope::releaseConsts(ConstStack* stack) noexcept -> void {
  if (m_constsPtr) {
    stack->release(m_constsCount);
    m_constsCount = 0;
    m_constsPtr   = nullptr;
  }
}

auto ExecScope::getConst(uint8_t id) noexcept -> Value {
  assert(m_constsPtr != nullptr);
  assert(id < m_constsCount);
  return *(m_constsPtr + id);
}

auto ExecScope::setConst(uint8_t id, Value value) noexcept -> void {
  assert(m_constsPtr != nullptr);
  assert(id < m_constsCount);
  *(m_constsPtr + id) = value;
}

} // namespace vm::internal
