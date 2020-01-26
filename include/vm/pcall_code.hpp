#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class PCallCode : uint8_t {
  PrintChar       = 10,
  PrintString     = 11,
  PrintStringLine = 12,

  ReadChar       = 21,
  ReadStringLine = 22,

  GetEnvVar = 30,

  Sleep  = 240,
  Assert = 241,
};

auto operator<<(std::ostream& out, const PCallCode& rhs) -> std::ostream&;

} // namespace vm
