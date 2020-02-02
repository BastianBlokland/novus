#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

enum class ResultCode : uint8_t {
  Ok              = 0,
  InvalidAssembly = 1,

  CallStackOverflow  = 10,
  ConstStackOverflow = 11,
  EvalStackOverflow  = 12,
  EvalStackNotEmpty  = 13,

  DivByZero    = 20,
  AssertFailed = 21
};

auto operator<<(std::ostream& out, const ResultCode& rhs) noexcept -> std::ostream&;

} // namespace vm
