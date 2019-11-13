#pragma once
#include "internal/exec_scope.hpp"
#include <stack>

namespace vm::internal {

class CallStack final {
public:
  CallStack()                         = default;
  CallStack(const CallStack& rhs)     = delete;
  CallStack(CallStack&& rhs) noexcept = delete;
  ~CallStack();

  auto operator=(const CallStack& rhs) -> CallStack& = delete;
  auto operator=(CallStack&& rhs) noexcept -> CallStack& = delete;

  [[nodiscard]] auto getTop() -> ExecScope*;

  auto push(const Program& program, uint32_t ipOffset) -> void;
  auto pop() -> bool;

private:
  std::stack<ExecScope> m_scopes;
};

} // namespace vm::internal
