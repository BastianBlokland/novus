#pragma once
#include <cstdint>
#include <iostream>

namespace novasm {

/*
 * Items in parentheses '()' on the left side of '->' are taken from the stack and on the right side
 * are pushed onto the stack.
 */

enum class PCallCode : uint8_t {
  EndiannessNative  = 1, // () -> (int) Get the native endianness of the platform.
  PlatformErrorCode = 2, // () -> (int) Get the last platform error, see notes at bottom of file.

  StreamCheckValid   = 10, // (stream)         -> (int)     Check if given stream is valid.
  StreamReadString   = 11, // (int, stream)    -> (string)  Read up to x bytes from a stream.
  StreamWriteString  = 12, // (string, stream) -> (int)     Write string, returns success.
  StreamSetOptions   = 13, // (int, stream)    -> (int)     Set options, returns success.
  StreamUnsetOptions = 14, // (int, stream)    -> (int)     Unset options, returns success.

  ProcessStart = 20, // (string)  -> (process) Start a new sys process from the given cmdline str.
  ProcessBlock = 21, // (process) -> (int)     Block until the process has exited, returns exitcode.
  ProcessOpenStream = 22, // (int, process) -> (stream)  Get a stream to stdin, stdout or stderr.
  ProcessGetId      = 23, // (process)      -> (long) Retrieve the native process id.
  ProcessSendSignal = 24, // (int, process) -> (int) Send a signal to the process, return success.

  FileOpenStream = 30, // (int, string) -> (stream)  Open a file at path with options.
  FileRemove     = 31, // (string)      -> (int)     Remove the file at path, returns success.

  TcpOpenCon      = 40, // (int, int, string) -> (stream) Open a connect to a remote addr and port.
  TcpStartServer  = 41, // (int, int, int)    -> (stream) Start a tcp-server at port.
  TcpAcceptCon    = 42, // (stream)           -> (stream) Accept a new connection from a tcp-server.
  TcpShutdown     = 43, // (stream)           -> (int) Shutdown tcp conn or server, returns success.
  IpLookupAddress = 45, // (int, string)      -> (string) Lookup an ip-address by host-name.

  ConsoleOpenStream = 50, // (int) -> (stream)  Get a stream to stdin, stdout or stderr.

  TermSetOptions   = 60, // (int, stream) -> (int)  Set terminal control options, returns success.
  TermUnsetOptions = 61, // (int, stream) -> (int)  Unset terminal control options, returns success.
  TermGetWidth     = 62, // (stream)      -> (int)  Get the width (num columns) of the terminal.
  TermGetHeight    = 63, // (stream)      -> (int)  Get the height (num rows) of the terminal.

  EnvGetArg        = 70, // (int)     -> (string) Get an environment argument by index.
  EnvGetArgCount   = 71, // ()        -> (int)    Get the amount of environment arguments provided.
  EnvGetVar        = 72, // (string)  -> (string) Get a environment variable by name.
  InteruptIsReq    = 73, // ()        -> (int)    Check if an interupt has been requested.
  InteruptResetReq = 74, // ()        -> (int)    Reset the 'is requested' flag, returns success.

  ClockMicroSinceEpoch = 80, // () -> (long) Get the elapsed microseconds since unix epoch.
  ClockNanoSteady      = 81, // () -> (long) Get process steady clock in nanoseconds.

  VersionRt       = 90, // () -> (string) Version of the runtime.
  VersionCompiler = 91, // () -> (string) Version of the compiler that created this assembly.

  PlatformCode   = 100, // () -> (int)    Platform id: Linux: 1, MacOs: 2, Windows: 3.
  WorkingDirPath = 101, // () -> (string) Get the path of the current working directory.
  RtPath         = 102, // () -> (string) Get the path of the runtime executable.
  ProgramPath    = 103, // () -> (string) Get the path of the currently running program.

  SleepNano = 240, // (long)         -> (int) Sleep the current executor for x nanoseconds.
  Assert    = 241, // (string, int)  -> (int) If condition is false: fail with message.
};

/* Platform errors
 * The following calls will set the error code that is returned from 'PlatformErrorCode':
 * - StreamReadString, error is set when an empty string is returned.
 * - StreamWriteString, error is set when false is returned.
 * - StreamSetOptions, error is set when false is returned.
 * - StreamUnsetOptions, error is set when false is returned.
 * - ProcessStart, error is set when an process with id -1 is returned.
 * - ProcessSendSignal, error is set when false is returned.
 * - FileOpenStream, error is set when an invalid stream is returned.
 * - FileRemove, error is set when false is returned.
 * - TcpOpenCon, error is set when an invalid stream is returned.
 * - TcpStartServer, error is set when an invalid stream is returned.
 * - TcpAcceptCon, error is set when an invalid stream is returned.
 * - TcpShutdown, error is set when false is returned.
 * - IpLookupAddress, error is set when empty string is returned.
 * - ConsoleOpenStream, error is set when an invalid stream is returned.
 * - SleepNano, error is set when false is returned.
 * - TermSetOptions, error is set when false is returned.
 * - TermUnsetOptions, error is set when false is returned.
 * - TermGetWidth, error is set when -1 is returned.
 * - TermGetHeight, error is set when -1 is returned.
 */

auto operator<<(std::ostream& out, const PCallCode& rhs) noexcept -> std::ostream&;

} // namespace novasm
