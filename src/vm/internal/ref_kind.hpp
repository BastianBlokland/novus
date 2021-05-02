#pragma once
#include <cstdint>

namespace vm::internal {

enum class RefKind : uint8_t {
  Atomic        = 0u,
  Struct        = 1U,
  Future        = 2U,
  String        = 3U,
  StringLink    = 4U,
  ULong         = 5U,
  StreamFile    = 6U,
  StreamConsole = 7U,
  StreamTcp     = 8U,
  StreamProcess = 9U,
  Process       = 10U,
  IOWatcher     = 11U,
};

} // namespace vm::internal
