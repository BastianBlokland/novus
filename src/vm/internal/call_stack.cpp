#include "internal/call_stack.hpp"

namespace vm::internal {

auto CallStack::getTop() -> ExecScope* { return &m_scopes.top(); }

auto CallStack::push(const Assembly& assembly, uint32_t ipOffset) -> void {
  m_scopes.push(ExecScope{assembly, ipOffset});
}

auto CallStack::pop() -> bool {
  m_scopes.pop();
  return !m_scopes.empty();
}

} // namespace vm::internal
