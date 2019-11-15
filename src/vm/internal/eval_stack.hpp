#pragma once
#include "internal/value.hpp"

namespace vm::internal {

class EvalStack final {
public:
  explicit EvalStack(unsigned int capacity);
  EvalStack(const EvalStack& rhs)     = delete;
  EvalStack(EvalStack&& rhs) noexcept = delete;
  ~EvalStack();

  auto operator=(const EvalStack& rhs) -> EvalStack& = delete;
  auto operator=(EvalStack&& rhs) noexcept -> EvalStack& = delete;

  [[nodiscard]] auto getSize() const noexcept -> unsigned int;

  auto push(Value value) -> void;
  auto peek() -> Value;
  auto pop() -> Value;

private:
  unsigned int m_capacity;
  Value* m_stack;
  Value* m_stackTop;
};

} // namespace vm::internal
