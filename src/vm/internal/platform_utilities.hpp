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

  TcpUnknownError              = 300,
  TcpInvalidSocket             = 301,
  TcpInvalidConnectionSocket   = 302,
  TcpInvalidServerSocket       = 303,
  TcpAlreadyInProcess          = 304,
  TcpNetworkDown               = 305,
  TcpSocketCouldNotBeAllocated = 306,
  TcpInvalidPort               = 307,
  TcpInvalidBacklog            = 308,
  TcpInvalidAddressFamily      = 309,
  TcpNoAccess                  = 310,
  TcpAddressInUse              = 311,
  TcpInvalidAddress            = 312,
  TcpNetworkUnreachable        = 313,
  TcpAddressFamilyNotSupported = 314,
  TcpConnectionRefused         = 315,
  TcpTimeout                   = 316,
  TcpAddressNotFound           = 317,
  TcpRemoteResetConnection     = 318,
  TcpSocketIsDead              = 319,
};

} // namespace vm::internal
