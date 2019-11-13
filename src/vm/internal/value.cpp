#include "internal/value.hpp"

namespace vm::internal {

auto Value::getInt() -> int32_t {
  return m_val.Int; // NOLINT: Union member access
}

auto intValue(int32_t val) -> Value {
  Value result;           // NOLINT: Uninitialized class
  result.m_val.Int = val; // NOLINT: Union member access
  return result;
}

} // namespace vm::internal
