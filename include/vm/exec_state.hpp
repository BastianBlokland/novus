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
  AllocFailed     = 4,
  DivByZero       = 5,
  AssertFailed    = 6,
};

auto operator<<(std::ostream& out, const ExecState& rhs) noexcept -> std::ostream&;

} // namespace vm
