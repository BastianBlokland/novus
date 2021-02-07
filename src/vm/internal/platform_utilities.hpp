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

  FileUnknownError    = 300,
  FileNoAccess        = 301,
  FileNotFound        = 302,
  FileInvalidFileName = 303,
  FilePathTooLong     = 304,
  FileDiskFull        = 305,
  FileLocked          = 306,

  TcpUnknownError              = 400,
  TcpInvalidSocket             = 401,
  TcpInvalidConnectionSocket   = 402,
  TcpInvalidServerSocket       = 403,
  TcpAlreadyInProcess          = 404,
  TcpNetworkDown               = 405,
  TcpSocketCouldNotBeAllocated = 406,
  TcpInvalidPort               = 407,
  TcpInvalidBacklog            = 408,
  TcpInvalidAddressFamily      = 409,
  TcpNoAccess                  = 410,
  TcpAddressInUse              = 411,
  TcpAddressUnavailable        = 412,
  TcpInvalidAddress            = 413,
  TcpNetworkUnreachable        = 414,
  TcpAddressFamilyNotSupported = 415,
  TcpConnectionRefused         = 416,
  TcpTimeout                   = 417,
  TcpAddressNotFound           = 418,
  TcpRemoteResetConnection     = 419,
  TcpSocketIsDead              = 420,

  SleepFailed = 500,
};

} // namespace vm::internal
