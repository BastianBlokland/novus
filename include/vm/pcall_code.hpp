#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class PCallCode : uint8_t {
  ConWriteChar       = 10,
  ConWriteString     = 11,
  ConWriteStringLine = 12,

  ConReadChar       = 21,
  ConReadStringLine = 22,

  GetEnvArg      = 30,
  GetEnvArgCount = 31,
  GetEnvVar      = 32,

  Sleep  = 240,
  Assert = 241,
};

auto operator<<(std::ostream& out, const PCallCode& rhs) -> std::ostream&;

} // namespace vm
