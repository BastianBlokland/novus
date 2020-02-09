#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class ExecState : int8_t {
  Paused          = -2,
  Running         = -1,
  Success         = 0,
  Aborted         = 1,
  InvalidAssembly = 2,
  StackOverflow   = 3,
  DivByZero       = 4,
  AssertFailed    = 5,
};

auto operator<<(std::ostream& out, const ExecState& rhs) noexcept -> std::ostream&;

} // namespace vm
