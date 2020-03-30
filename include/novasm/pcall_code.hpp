#pragma once
#include <cstdint>
#include <iostream>

namespace novasm {

/*
  Items in parentheses '()' on the left side of '->' are taken from the stack and on the right side
  are pushed onto the stack.
*/

enum class PCallCode : uint8_t {
  StreamOpenFile    = 10, // (int, string)    -> (stream)  Open a file at path with options.
  StreamOpenConsole = 11, // (int)            -> (stream)  Get a stream to stdin, stdout or stderr.
  StreamCheckValid  = 12, // (stream)         -> (int)     Check if given stream is valid.
  StreamReadString  = 13, // (int, stream)    -> (string)  Read up to x bytes from a stream.
  StreamReadChar    = 14, // (stream)         -> (int)     Read a single character from a stream.
  StreamWriteString = 15, // (string, stream) -> (int)     Write string, returns success.
  StreamWriteChar   = 16, // (int, stream)    -> (int)     Write character, returns success.
  StreamFlush       = 17, // (stream)         -> (stream)  Flush any unwritten data to output.

  FileRemove = 40, // (string)         -> (int)     Remove the file at path, returns success.

  GetEnvArg      = 50, // (int)     -> (string) Get an environment argument by index.
  GetEnvArgCount = 51, // ()        -> (int)    Get the amount of environment arguments provided.
  GetEnvVar      = 52, // (string)  -> (string) Get a environment variable by name.

  ClockMicroSinceEpoch = 60, // () -> (long) Get the elapsed microseconds since unix epoch.
  ClockNanoSteady      = 61, // () -> (long) Get process steady clock in nanoseconds.

  SleepNano = 240, // (long)         -> (long) Sleep the current executor for x nanoseconds.
  Assert    = 241, // (string, int)  -> (int) If condition is false: fail with message.
};

auto operator<<(std::ostream& out, const PCallCode& rhs) noexcept -> std::ostream&;

} // namespace novasm
