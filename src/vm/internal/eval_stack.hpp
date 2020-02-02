#pragma once
#include "internal/value.hpp"
#include "vm/result_code.hpp"
#include <array>
#include <cassert>

namespace vm::internal {

template <unsigned int Capacity>
class EvalStack final {
public:
  EvalStack() noexcept :
      m_stack{}, m_stackNext{m_stack.data()}, m_stackMax{m_stack.data() + Capacity} {}
  EvalStack(const EvalStack& rhs) = delete;
  EvalStack(EvalStack&& rhs)      = delete;
  ~EvalStack() noexcept           = default;

  auto operator=(const EvalStack& rhs) -> EvalStack& = delete;
  auto operator=(EvalStack&& rhs) -> EvalStack& = delete;

  [[nodiscard]] inline auto getSize() const noexcept -> unsigned int {
    return m_stackNext - m_stack.data();
  }

  [[nodiscard]] inline auto push(Value value) noexcept -> ResultCode {
    if (m_stackNext == m_stackMax) {
      return ResultCode::EvalStackOverflow;
    }
    *m_stackNext = value;
    ++m_stackNext;
    return ResultCode::Ok;
  }

  inline auto peek() noexcept -> Value { return *(m_stackNext - 1); }

  inline auto pop() noexcept -> Value {
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
