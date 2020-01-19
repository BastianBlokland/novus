#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class PCallCode : uint8_t {
  PrintChar       = 0,
  PrintString     = 1,
  PrintStringLine = 2,
  ReadChar        = 3,
  ReadStringLine  = 4,
  Sleep           = 5,
};

auto operator<<(std::ostream& out, const PCallCode& rhs) -> std::ostream&;

} // namespace vm
