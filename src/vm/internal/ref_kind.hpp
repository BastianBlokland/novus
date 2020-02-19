#pragma once
#include <cstdint>

namespace vm::internal {

enum class RefKind : uint8_t {
  Struct = 0U,
  Future = 1U,
  String = 2U,
  Long   = 3U,
};

} // namespace vm::internal
