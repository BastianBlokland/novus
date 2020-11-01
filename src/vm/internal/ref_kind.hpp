#pragma once
#include <cstdint>

namespace vm::internal {

enum class RefKind : uint8_t {
  Struct        = 0U,
  Future        = 1U,
  String        = 2U,
  StringLink    = 3U,
  Long          = 4U,
  StreamFile    = 5U,
  StreamConsole = 6U,
  StreamTcp     = 7U,
  Process       = 8U,
};

} // namespace vm::internal
