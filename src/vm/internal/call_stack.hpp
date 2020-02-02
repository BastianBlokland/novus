#pragma once
#include "internal/exec_scope.hpp"
#include "vm/result_code.hpp"

namespace vm::internal {

class CallStack final {
public:
  explicit CallStack(unsigned int capacity) noexcept;
  CallStack(const CallStack& rhs) = delete;
  CallStack(CallStack&& rhs)      = delete;
  ~CallStack() noexcept;

  auto operator=(const CallStack& rhs) -> CallStack& = delete;
  auto operator=(CallStack&& rhs) -> CallStack& = delete;

  [[nodiscard]] auto getTop() noexcept -> ExecScope*;

  [[nodiscard]] auto push(const uint8_t* ip) noexcept -> ResultCode;
  auto pop() noexcept -> bool;

private:
  ExecScope* m_stack;
  ExecScope* m_stackTop;
  ExecScope* m_stackMax;
};

} // namespace vm::internal
