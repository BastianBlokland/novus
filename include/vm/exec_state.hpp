#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class ExecState : int8_t {
  Running         = -1,
  Success         = 0,
  InvalidAssembly = 1,
  StackOverflow   = 2,
  DivByZero       = 3,
  AssertFailed    = 4,
};

auto operator<<(std::ostream& out, const ExecState& rhs) noexcept -> std::ostream&;

} // namespace vm
