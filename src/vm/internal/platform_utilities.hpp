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
  TcpAddressUnavailable        = 312,
  TcpInvalidAddress            = 313,
  TcpNetworkUnreachable        = 314,
  TcpAddressFamilyNotSupported = 315,
  TcpConnectionRefused         = 316,
  TcpTimeout                   = 317,
  TcpAddressNotFound           = 318,
  TcpRemoteResetConnection     = 319,
  TcpSocketIsDead              = 320,
};

} // namespace vm::internal
