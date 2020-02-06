#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class ResultCode : uint8_t {
  Ok              = 0,
  DivByZero       = 1,
  StackOverflow   = 2,
  AssertFailed    = 3,
  InvalidAssembly = 4,
};

auto operator<<(std::ostream& out, const ResultCode& rhs) noexcept -> std::ostream&;

} // namespace vm
