#include "internal/call_stack.hpp"
#include "vm/exceptions/call_stack_overflow.hpp"

namespace vm::internal {

CallStack::CallStack(unsigned int capacity) :
    m_stack{new ExecScope[capacity]}, m_stackTop{m_stack}, m_stackMax{m_stack + capacity} {}

CallStack::~CallStack() { delete[] m_stack; }

auto CallStack::getTop() -> ExecScope* { return m_stackTop; }

auto CallStack::push(const uint8_t* ip) -> void {
  ++m_stackTop;
  if (m_stackTop == m_stackMax) {
    throw exceptions::CallStackOverflow{};
  }
  m_stackTop->m_ip          = ip;
  m_stackTop->m_constsCount = 0U;
  m_stackTop->m_constsPtr   = nullptr;
}

auto CallStack::pop() -> bool {
  assert(m_stackTop - m_stack != 0);
  m_stackTop--;
  return m_stackTop != m_stack;
}

} // namespace vm::internal
