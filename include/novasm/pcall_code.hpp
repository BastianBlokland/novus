#pragma once
#include <cstdint>
#include <iostream>

namespace novasm {

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

  StreamOpenFile   = 31, // (int, string)    -> (stream)  Open a file at path with options.
  StreamCheckValid = 32, // (stream)         -> (int)     Check if given stream is valid.
  StreamRead       = 33, // (int, stream)    -> (string)  Read up to x bytes from a stream.
  StreamWrite      = 34, // (string, stream) -> (int)     Write to a stream, returns success.
  StreamFlush      = 35, // (stream)         -> (stream)  Flush any unwritten data to output device.

  GetEnvArg      = 40, // (int)     -> (string) Get an environment argument by index.
  GetEnvArgCount = 41, // ()        -> (int)    Get the amount of environment arguments provided.
  GetEnvVar      = 42, // (string)  -> (string) Get a environment variable by name.

  ClockMicroSinceEpoch = 50, // () -> (long) Get the elapsed microseconds since unix epoch.
  ClockNanoSteady      = 51, // () -> (long) Get process steady clock in nanoseconds.

  SleepNano = 240, // (long)         -> (long) Sleep the current executor for x nanoseconds.
  Assert    = 241, // (string, int)  -> (int) If condition is false: fail with message.
};

auto operator<<(std::ostream& out, const PCallCode& rhs) noexcept -> std::ostream&;

} // namespace novasm
