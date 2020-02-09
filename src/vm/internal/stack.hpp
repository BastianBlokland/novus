#pragma once
#include "internal/value.hpp"
#include <array>
#include <cassert>

namespace vm::internal {

template <unsigned int Capacity>
class Stack final {
public:
  Stack() noexcept :
      m_stack{}, m_stackNext{m_stack.data()}, m_stackMax{m_stack.data() + Capacity} {}
  Stack(const Stack& rhs) = delete;
  Stack(Stack&& rhs)      = delete;
  ~Stack() noexcept       = default;

  auto operator=(const Stack& rhs) -> Stack& = delete;
  auto operator=(Stack&& rhs) -> Stack& = delete;

  [[nodiscard]] inline auto getBottom() noexcept -> Value* { return m_stack.data(); }

  [[nodiscard]] inline auto getTop() const noexcept -> Value* { return m_stackNext - 1; }

  [[nodiscard]] inline auto getNext() const noexcept -> Value* { return m_stackNext; }

  [[nodiscard]] inline auto isEmpty() const noexcept -> bool {
    return m_stackNext == m_stack.data();
  }

  [[nodiscard]] inline auto getSize() const noexcept -> unsigned int {
    return m_stackNext - m_stack.data();
  }

  inline auto rewindToNext(Value* next) noexcept -> void {
    assert(next <= getNext()); // Not allowed to go forwards.
    m_stackNext = next;
  }

  inline auto rewindToTop(Value* top) noexcept -> void {
    assert(top <= getTop()); // Not allowed to go forwards.
    m_stackNext = top + 1;
  }

  [[nodiscard]] inline auto alloc(unsigned int amount) noexcept -> bool {
    assert(amount != 0);
    m_stackNext += amount;
    return m_stackNext < m_stackMax;
  }

  inline auto push(Value value) noexcept -> bool {
    *m_stackNext++ = value;
    return m_stackNext < m_stackMax;
  }

  inline auto peek() noexcept -> Value { return *getTop(); }

  inline auto pop() noexcept -> Value {
    assert(m_stackNext - m_stack.data() != 0);
    return *--m_stackNext;
  }

private:
  std::array<Value, Capacity + 1> m_stack; // + 1 to be able to delay the bounds check.
  Value* m_stackNext;
  Value* m_stackMax;
};

using BasicStack = Stack<10240>;

} // namespace vm::internal
