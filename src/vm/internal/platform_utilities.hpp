#pragma once
#include "os_include.hpp"
#include <cstdint>

namespace vm::internal {

class StringRef;
class RefAllocator;

enum class Endianness : uint8_t {
  Little = 0,
  Big    = 1,
};

[[nodiscard]] auto getEndianness() noexcept -> Endianness;

enum class PlatformError : uint32_t {
  None    = 0,
  Unknown = 1,

  FeatureNetworkNotEnabled = 100,

  StreamOptionsNotSupported = 200,
  StreamNoDataAvailable     = 201,
  StreamReadNotSupported    = 202,
  StreamWriteNotSupported   = 203,
  StreamInvalid             = 204,

  ProcessUnknownError        = 300,
  ProcessInvalid             = 301,
  ProcessInvalidCmdLine      = 302,
  ProcessFailedToCreatePipes = 303,
  ProcessNoAccess            = 304,
  ProcessExecutableNotFound  = 305,
  ProcessExecutableInvalid   = 306,
  ProcessLimitReached        = 307,
  ProcessInvalidSignal       = 308,

  ConsoleUnknownError           = 400,
  ConsoleNotPresent             = 401,
  ConsoleNoTerminal             = 402,
  ConsoleFailedToGetTermInfo    = 403,
  ConsoleFailedToUpdateTermInfo = 404,
  ConsoleNoLongerAvailable      = 405,

  FileUnknownError      = 500,
  FileNoAccess          = 501,
  FileNotFound          = 502,
  FileInvalidFileName   = 503,
  FilePathTooLong       = 504,
  FileDiskFull          = 505,
  FileLocked            = 506,
  FileIsDirectory       = 507,
  FileIsNotDirectory    = 508,
  FileDirectoryNotEmpty = 509,
  FileAlreadyExists     = 510,
  FileTooManyOpenFiles  = 511,

  TcpUnknownError              = 600,
  TcpInvalidSocket             = 601,
  TcpInvalidServerSocket       = 602,
  TcpAlreadyInProcess          = 603,
  TcpNetworkDown               = 604,
  TcpSocketCouldNotBeAllocated = 605,
  TcpInvalidPort               = 606,
  TcpInvalidBacklog            = 607,
  TcpInvalidAddressFamily      = 608,
  TcpNoAccess                  = 609,
  TcpAddressInUse              = 610,
  TcpAddressUnavailable        = 611,
  TcpInvalidAddress            = 612,
  TcpNetworkUnreachable        = 613,
  TcpAddressFamilyNotSupported = 614,
  TcpConnectionRefused         = 615,
  TcpTimeout                   = 616,
  TcpAddressNotFound           = 617,
  TcpRemoteResetConnection     = 618,
  TcpSocketIsDead              = 619,

  IOWatcherUnknownError       = 700,
  IOWatcherFileAlreadyWatched = 701,
  IOWatcherNotSupported       = 702,

  SleepFailed = 800,
};

auto setupPlatformUtilities() noexcept -> void;
auto teardownPlatformUtilities() noexcept -> void;

[[nodiscard]] auto clockMicroSinceEpoch() noexcept -> int64_t;

[[nodiscard]] auto clockNanoSteady() noexcept -> int64_t;

// Returns the local timezone offset in minutes.
[[nodiscard]] auto clockTimezoneOffset() noexcept -> int32_t;

[[nodiscard]] auto platformHasEnv(const StringRef* name) noexcept -> bool;
[[nodiscard]] auto platformGetEnv(const StringRef* name, RefAllocator* refAlloc) noexcept
    -> StringRef*;

[[nodiscard]] auto platformWorkingDirPath(RefAllocator* refAlloc) noexcept -> StringRef*;
[[nodiscard]] auto platformExecPath(RefAllocator* refAlloc) noexcept -> StringRef*;

#if defined(_WIN32)
[[nodiscard]] inline auto winFileTimeToMicroSinceEpoch(const FILETIME& fileTime) noexcept
    -> int64_t {

  // Windows FILETIME is in 100 ns ticks since January 1 1601.
  constexpr int64_t winEpochToUnixEpoch = 116'444'736'000'000'000LL;
  constexpr int64_t winTickToMicro      = 10LL;

  LARGE_INTEGER winTicks;
  winTicks.LowPart  = fileTime.dwLowDateTime;
  winTicks.HighPart = fileTime.dwHighDateTime;

  return (winTicks.QuadPart - winEpochToUnixEpoch) / winTickToMicro;
}
#endif

} // namespace vm::internal
