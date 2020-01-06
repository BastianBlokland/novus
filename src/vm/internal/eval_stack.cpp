#include "internal/eval_stack.hpp"
#include "vm/exceptions/eval_stack_overflow.hpp"
#include <cassert>

namespace vm::internal {

EvalStack::EvalStack(unsigned int capacity) :
    m_stack{new Value[capacity]}, m_stackNext{m_stack}, m_stackMax{m_stack + capacity} {}

EvalStack::~EvalStack() { delete[] m_stack; }

auto EvalStack::getSize() const noexcept -> unsigned int { return m_stackNext - m_stack; }

auto EvalStack::push(const Value value) -> void {
  if (m_stackNext == m_stackMax) {
    throw exceptions::EvalStackOverflow{};
  }
  *m_stackNext = value;
  ++m_stackNext;
}

auto EvalStack::peek() -> Value { return *(m_stackNext - 1); }

auto EvalStack::pop() -> Value {
  assert(m_stackNext - m_stack != 0);
  m_stackNext--;
  return *m_stackNext;
}

} // namespace vm::internal
