#pragma once
#include "internal/exec_scope.hpp"

namespace vm::internal {

class CallStack final {
public:
  explicit CallStack(unsigned int capacity);
  CallStack(const CallStack& rhs)     = delete;
  CallStack(CallStack&& rhs) noexcept = delete;
  ~CallStack();

  auto operator=(const CallStack& rhs) -> CallStack& = delete;
  auto operator=(CallStack&& rhs) noexcept -> CallStack& = delete;

  [[nodiscard]] auto getTop() -> ExecScope*;

  auto push(const uint8_t* ip) -> void;
  auto pop() -> bool;

private:
  ExecScope* m_stack;
  ExecScope* m_stackTop;
  ExecScope* m_stackMax;
};

} // namespace vm::internal
