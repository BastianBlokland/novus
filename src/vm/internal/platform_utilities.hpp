#pragma once
#include <cstdint>

namespace vm::internal {

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

  ConsoleUnknownError           = 300,
  ConsoleNotPresent             = 301,
  ConsoleNoTerminal             = 302,
  ConsoleFailedToGetTermInfo    = 303,
  ConsoleFailedToUpdateTermInfo = 304,

  FileUnknownError    = 400,
  FileNoAccess        = 401,
  FileNotFound        = 402,
  FileInvalidFileName = 403,
  FilePathTooLong     = 404,
  FileDiskFull        = 405,
  FileLocked          = 406,

  TcpUnknownError              = 500,
  TcpInvalidSocket             = 501,
  TcpInvalidServerSocket       = 502,
  TcpAlreadyInProcess          = 503,
  TcpNetworkDown               = 504,
  TcpSocketCouldNotBeAllocated = 505,
  TcpInvalidPort               = 506,
  TcpInvalidBacklog            = 507,
  TcpInvalidAddressFamily      = 508,
  TcpNoAccess                  = 509,
  TcpAddressInUse              = 510,
  TcpAddressUnavailable        = 511,
  TcpInvalidAddress            = 512,
  TcpNetworkUnreachable        = 513,
  TcpAddressFamilyNotSupported = 514,
  TcpConnectionRefused         = 515,
  TcpTimeout                   = 516,
  TcpAddressNotFound           = 517,
  TcpRemoteResetConnection     = 518,
  TcpSocketIsDead              = 519,

  SleepFailed = 600,
};

} // namespace vm::internal
