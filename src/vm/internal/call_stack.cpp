#include "internal/call_stack.hpp"

namespace vm::internal {

CallStack::CallStack(unsigned int capacity) noexcept :
    m_stack{new ExecScope[capacity]}, m_stackTop{m_stack}, m_stackMax{m_stack + capacity} {}

CallStack::~CallStack() noexcept { delete[] m_stack; }

auto CallStack::getTop() noexcept -> ExecScope* { return m_stackTop; }

auto CallStack::push(const uint8_t* ip) noexcept -> ResultCode {
  ++m_stackTop;
  if (m_stackTop == m_stackMax) {
    return ResultCode::CallStackOverflow;
  }
  m_stackTop->m_ip          = ip;
  m_stackTop->m_constsCount = 0U;
  m_stackTop->m_constsPtr   = nullptr;
  return ResultCode::Ok;
}

auto CallStack::pop() noexcept -> bool {
  assert(m_stackTop - m_stack != 0);
  m_stackTop--;
  return m_stackTop != m_stack;
}

} // namespace vm::internal
