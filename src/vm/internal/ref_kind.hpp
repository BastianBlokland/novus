#pragma once
#include <cstdint>

namespace vm::internal {

enum class RefKind : uint8_t {
  String = 0U,
  Struct = 1U,
  Future = 2U,
};

} // namespace vm::internal
