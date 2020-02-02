#include "internal/const_stack.hpp"
#include <cassert>

namespace vm::internal {

ConstStack::ConstStack(unsigned int capacity) noexcept :
    m_stack{new Value[capacity]}, m_stackTop{m_stack}, m_stackMax{m_stack + capacity} {}

ConstStack::~ConstStack() noexcept { delete[] m_stack; }

auto ConstStack::getSize() const noexcept -> unsigned int { return m_stackTop - m_stack; }

auto ConstStack::reserve(unsigned int amount, Value** result) noexcept -> ResultCode {
  *result = m_stackTop;
  m_stackTop += amount;
  if (m_stackTop >= m_stackMax) {
    return ResultCode::ConstStackOverflow;
  }
  return ResultCode::Ok;
}

auto ConstStack::release(unsigned int amount) noexcept -> void {
  assert(m_stackTop - m_stack >= amount);
  m_stackTop -= amount;
}

} // namespace vm::internal
