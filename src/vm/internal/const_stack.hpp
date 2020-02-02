#pragma once
#include "internal/value.hpp"
#include "vm/result_code.hpp"

namespace vm::internal {

class ConstStack final {
public:
  explicit ConstStack(unsigned int capacity) noexcept;
  ConstStack(const ConstStack& rhs) = delete;
  ConstStack(ConstStack&& rhs)      = delete;
  ~ConstStack() noexcept;

  auto operator=(const ConstStack& rhs) -> ConstStack& = delete;
  auto operator=(ConstStack&& rhs) -> ConstStack& = delete;

  [[nodiscard]] auto getSize() const noexcept -> unsigned int;

  [[nodiscard]] auto reserve(unsigned int amount, Value** result) noexcept -> ResultCode;
  auto release(unsigned int amount) noexcept -> void;

private:
  Value* m_stack;
  Value* m_stackTop;
  Value* m_stackMax;
};

} // namespace vm::internal
