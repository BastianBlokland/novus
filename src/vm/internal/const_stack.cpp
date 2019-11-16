#include "internal/const_stack.hpp"
#include "vm/exceptions/const_stack_overflow.hpp"
#include <cassert>

namespace vm::internal {

ConstStack::ConstStack(unsigned int capacity) :
    m_capacity{capacity}, m_stack{new Value[capacity]}, m_stackTop{m_stack} {}

ConstStack::~ConstStack() { delete[] m_stack; }

auto ConstStack::getSize() const noexcept -> unsigned int { return m_stackTop - m_stack; }

auto ConstStack::reserve(unsigned int amount) -> Value* {
  if ((m_stackTop - m_stack) + amount > m_capacity) {
    throw exceptions::ConstStackOverflow{};
  }
  const auto ret = m_stackTop;
  m_stackTop += amount;
  return ret;
}

auto ConstStack::release(unsigned int amount) -> void {
  assert(m_stackTop - m_stack >= amount);
  m_stackTop -= amount;
}

} // namespace vm::internal
