#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class ExecState : int8_t {
  Paused          = -2,
  Running         = -1,
  Success         = 0,
  Failed          = 1,
  Aborted         = 2,
  InvalidAssembly = 3,
  StackOverflow   = 4,
  AllocFailed     = 5,
  DivByZero       = 6,
  AssertFailed    = 7,
};

auto operator<<(std::ostream& out, const ExecState& rhs) noexcept -> std::ostream&;

} // namespace vm
