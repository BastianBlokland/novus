#pragma once
#include "internal/value.hpp"
#include "vm/exceptions/eval_stack_overflow.hpp"
#include <array>
#include <cassert>

namespace vm::internal {

template <unsigned int Capacity>
class EvalStack final {
public:
  EvalStack() : m_stack{}, m_stackNext{m_stack.data()}, m_stackMax{m_stack.data() + Capacity} {}
  EvalStack(const EvalStack& rhs)     = delete;
  EvalStack(EvalStack&& rhs) noexcept = delete;
  ~EvalStack()                        = default;

  auto operator=(const EvalStack& rhs) -> EvalStack& = delete;
  auto operator=(EvalStack&& rhs) noexcept -> EvalStack& = delete;

  [[nodiscard]] inline auto getSize() const noexcept -> unsigned int {
    return m_stackNext - m_stack.data();
  }

  inline auto push(Value value) -> void {
    if (m_stackNext == m_stackMax) {
      throw exceptions::EvalStackOverflow{};
    }
    *m_stackNext = value;
    ++m_stackNext;
  }

  inline auto peek() -> Value { return *(m_stackNext - 1); }

  inline auto pop() -> Value {
    assert(m_stackNext - m_stack.data() != 0);
    m_stackNext--;
    return *m_stackNext;
  }

private:
  std::array<Value, Capacity> m_stack;
  Value* m_stackNext;
  Value* m_stackMax;
};

} // namespace vm::internal
