#include "internal/eval_stack.hpp"
#include "vm/exceptions/eval_stack_overflow.hpp"
#include <cassert>

namespace vm::internal {

EvalStack::EvalStack(unsigned int capacity) :
    m_capacity{capacity}, m_stack{new Value[capacity]}, m_stackTop{m_stack} {}

EvalStack::~EvalStack() { delete[] m_stack; }

auto EvalStack::getSize() const noexcept -> unsigned int { return m_stackTop - m_stack; }

auto EvalStack::push(const Value value) -> void {
  // TODO(bastian): Reduce the cost by checking if enough stack-space is left before running a
  // function instead of checking on every push.
  if (m_stackTop - m_stack >= m_capacity) {
    throw exceptions::EvalStackOverflow{};
  }
  *m_stackTop = value;
  ++m_stackTop;
}

auto EvalStack::peek() -> Value { return *m_stackTop; }

auto EvalStack::pop() -> Value {
  assert(m_stackTop - m_stack != 0);
  m_stackTop--;
  return *m_stackTop;
}

} // namespace vm::internal
