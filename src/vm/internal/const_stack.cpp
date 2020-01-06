#include "internal/const_stack.hpp"
#include "vm/exceptions/const_stack_overflow.hpp"
#include <cassert>

namespace vm::internal {

ConstStack::ConstStack(unsigned int capacity) :
    m_stack{new Value[capacity]}, m_stackTop{m_stack}, m_stackMax{m_stack + capacity} {}

ConstStack::~ConstStack() { delete[] m_stack; }

auto ConstStack::getSize() const noexcept -> unsigned int { return m_stackTop - m_stack; }

auto ConstStack::reserve(unsigned int amount) -> Value* {
  const auto cur = m_stackTop;
  m_stackTop += amount;
  if (m_stackTop >= m_stackMax) {
    throw exceptions::ConstStackOverflow{};
  }
  return cur;
}

auto ConstStack::release(unsigned int amount) -> void {
  assert(m_stackTop - m_stack >= amount);
  m_stackTop -= amount;
}

} // namespace vm::internal
