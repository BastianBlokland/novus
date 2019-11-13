#pragma once
#include <cstdint>

namespace vm::internal {

class Value final {
  friend auto intValue(int32_t val) -> Value;

public:
  auto getInt() -> int32_t;

private:
  union {
    int32_t Int;
  } m_val;
};

auto intValue(int32_t val) -> Value;

} // namespace vm::internal
