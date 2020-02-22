#pragma once
#include <cstdint>
#include <iostream>

namespace vm {

/*
  Items in parentheses '()' on the left side of '->' are taken from the stack and on the right side
  are pushed onto the stack.
*/

enum class PCallCode : uint8_t {
  ConWriteChar       = 10, // (int)     -> (int)      Write a (ascii) char to the console.
  ConWriteString     = 11, // (string)  -> (string)   Write a string to the console.
  ConWriteStringLine = 12, // (string)  -> (string)   Write a string followed by newline to console.

  ConReadChar       = 21, // () -> (int)    Read a (ascii) char from the console.
  ConReadStringLine = 22, // () -> (string) Read a string from the console.

  GetEnvArg      = 30, // (int)     -> (string) Get an environment argument by index.
  GetEnvArgCount = 31, // ()        -> (int)    Get the amount of environment arguments provided.
  GetEnvVar      = 32, // (string)  -> (string) Get a environment variable by name.

  ClockNanoSinceEpoch = 40, // () -> (long) Get the elapsed nanoseconds since unix epoch.
  ClockNanoSteady     = 41, // () -> (long) Get the elapsed nanoseconds in the process steady clock.

  SleepNano = 240, // (long)         -> (long) Sleep the current executor for x nanoseconds.
  Assert    = 241, // (string, int)  -> (int) If condition is false: fail with message.
};

auto operator<<(std::ostream& out, const PCallCode& rhs) noexcept -> std::ostream&;

} // namespace vm
