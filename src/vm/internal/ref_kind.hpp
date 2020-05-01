#pragma once
#include <cstdint>

namespace vm::internal {

enum class RefKind : uint8_t {
  Struct     = 0U,
  Future     = 1U,
  String     = 2U,
  StringLink = 3U,
  Long       = 4U,
  Stream     = 5U,
};

} // namespace vm::internal
