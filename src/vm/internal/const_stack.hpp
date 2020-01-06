#pragma once
#include "internal/value.hpp"

namespace vm::internal {

class ConstStack final {
public:
  explicit ConstStack(unsigned int capacity);
  ConstStack(const ConstStack& rhs)     = delete;
  ConstStack(ConstStack&& rhs) noexcept = delete;
  ~ConstStack();

  auto operator=(const ConstStack& rhs) -> ConstStack& = delete;
  auto operator=(ConstStack&& rhs) noexcept -> ConstStack& = delete;

  [[nodiscard]] auto getSize() const noexcept -> unsigned int;

  auto reserve(unsigned int amount) -> Value*;
  auto release(unsigned int amount) -> void;

private:
  Value* m_stack;
  Value* m_stackTop;
  Value* m_stackMax;
};

} // namespace vm::internal
