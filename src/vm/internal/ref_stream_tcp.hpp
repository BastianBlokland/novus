#pragma once
#include "internal/fd_utilities.hpp"
#include "internal/os_include.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "internal/settings.hpp"
#include "internal/thread.hpp"
#include "intrinsics.hpp"
#include <atomic>
#include <cerrno>
#include <cstring>

#if defined(_WIN32)

#define OS_SOCKET SOCKET
#define OS_SOCKET_VALID(SOCK) (SOCK != INVALID_SOCKET)
#define OS_INVALID_SOCKET INVALID_SOCKET
#define OS_SOCKET_ERR WSAGetLastError()

#else // !_WIN32

#define OS_SOCKET int
#define OS_SOCKET_VALID(SOCK) (SOCK >= 0)
#define OS_INVALID_SOCKET -1
#define OS_SOCKET_ERR errno

#endif // !_WIN32

namespace vm::internal {

constexpr int32_t defaultConnectionBacklog = 64;
constexpr int32_t receiveTimeoutSeconds    = 15;

enum class TcpStreamType : uint8_t {
  Server     = 0, // Server cannot be used for sending or receiving but can accept new connections.
  Connection = 1, // Connections are be used for sending and receiving.
};

enum class TcpStreamState : uint8_t {
  Valid  = 0,
  Failed = 1,
  Closed = 2,
};

enum class IpAddressFamily : uint8_t {
  IpV4 = 0,
  IpV6 = 1,
};

inline auto isIpFamilyValid(IpAddressFamily family) noexcept {
  switch (family) {
  case IpAddressFamily::IpV4:
  case IpAddressFamily::IpV6:
    return true;
  }
  return false;
}

inline auto getIpFamilyCode(IpAddressFamily family) noexcept {
  switch (family) {
  case IpAddressFamily::IpV4:
    return AF_INET;
  case IpAddressFamily::IpV6:
    return AF_INET6;
  }
  return AF_UNSPEC;
}

inline auto getIpAddressSize(IpAddressFamily family) noexcept -> size_t {
  switch (family) {
  case IpAddressFamily::IpV4:
    return 4u;
  case IpAddressFamily::IpV6:
    return 16u;
  }
  return 0u;
}

auto configureSocket(OS_SOCKET sock) noexcept -> void;

auto getTcpPlatformError() noexcept -> PlatformError;

// Tcp implementation of the 'stream' interface.
// Note: To avoid needing a vtable there is no abstract 'Stream' class but instead there are wrapper
// functions that dispatch based on the 'RefKind' (see stream_utilities.hpp).
class TcpStreamRef final : public Ref {
  friend class RefAllocator;

public:
  TcpStreamRef(const TcpStreamRef& rhs) = delete;
  TcpStreamRef(TcpStreamRef&& rhs)      = delete;
  ~TcpStreamRef() noexcept {
    // Close the socket if its still open.
    const bool isClosed = m_state.load(std::memory_order_acquire) == TcpStreamState::Closed;
    if (OS_SOCKET_VALID(m_socket) && !isClosed) {
#if defined(_WIN32)
      ::closesocket(m_socket);
#else // !_WIN32
      ::close(m_socket);
#endif // !_WIN32
    }
  }

  auto operator=(const TcpStreamRef& rhs) -> TcpStreamRef& = delete;
  auto operator=(TcpStreamRef&& rhs) -> TcpStreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::StreamTcp; }

  [[nodiscard]] auto isValid() noexcept -> bool {
    return OS_SOCKET_VALID(m_socket) &&
        m_state.load(std::memory_order_acquire) == TcpStreamState::Valid;
  }

  auto shutdown() noexcept -> bool {
    if (m_type == TcpStreamType::Connection) {
      // Connection sockets we shutdown.
#if defined(__APPLE__)
      return ::shutdown(m_socket, SHUT_RDWR) == 0;
#elif defined(_WIN32) // !__APPLE__
      return ::shutdown(m_socket, SD_BOTH) == 0;
#else // !_WIN32 && !__APPLE__
      return ::shutdown(m_socket, SHUT_RDWR) == 0;
#endif // !_WIN32 && !__APPLE__
    }

    // For server sockets there is no such thing so we already close the socket.
    const auto prevState = m_state.exchange(TcpStreamState::Closed, std::memory_order_acq_rel);
    if (OS_SOCKET_VALID(m_socket) && prevState != TcpStreamState::Closed) {
#if defined(_WIN32)
      return ::closesocket(m_socket) == 0;
#else // !_WIN32
      return ::close(m_socket) == 0;
#endif // !_WIN32
    }
    return false; // Already closed before.
  }

  auto readString(ExecutorHandle* execHandle, PlatformError* pErr, StringRef* str) noexcept
      -> bool {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      *pErr = PlatformError::TcpInvalidConnectionSocket;
      str->updateSize(0);
      return false;
    }
    if (unlikely(str->getSize() == 0)) {
      return true;
    }

    execHandle->setState(ExecState::Paused);

    int bytesRead = -1;
    while (m_state.load(std::memory_order_acquire) == TcpStreamState::Valid) {
      bytesRead = ::recv(m_socket, str->getCharDataPtr(), str->getSize(), 0);
      if (bytesRead >= 0) {
        break; // No error while reading.
      }

      // Retry the send for certain errors.
#if defined(_WIN32)
      const bool shouldRetry = OS_SOCKET_ERR == WSAEINTR;
#else // !_WIN32
      const bool shouldRetry = OS_SOCKET_ERR == EINTR;
#endif // !_WIN32
      if (!shouldRetry) {
        break;
      }
      threadYield(); // Yield between retries.
    }

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false; // Aborted.
    }

    if (bytesRead < 0) {
      if (errno == EAGAIN) {
        *pErr = PlatformError::TcpTimeout;
      } else {
        *pErr = getTcpPlatformError();
      }
      str->updateSize(0);
      m_state.store(TcpStreamState::Failed, std::memory_order_release);
      return false;
    }
    if (bytesRead == 0) {
      *pErr = PlatformError::StreamNoDataAvailable;
      str->updateSize(0);
      return false;
    }
    str->updateSize(bytesRead);
    return true;
  }

  auto readChar(ExecutorHandle* execHandle, PlatformError* pErr) noexcept -> char {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      *pErr = PlatformError::TcpInvalidConnectionSocket;
      return '\0';
    }

    execHandle->setState(ExecState::Paused);

    char res      = '\0';
    int bytesRead = -1;
    while (m_state.load(std::memory_order_acquire) == TcpStreamState::Valid) {
      bytesRead = ::recv(m_socket, &res, 1, 0);
      if (bytesRead >= 0) {
        break; // No error while reading.
      }

      // Retry the send for certain errors.
#if defined(_WIN32)
      const bool shouldRetry = OS_SOCKET_ERR == WSAEINTR;
#else // !_WIN32
      const bool shouldRetry = OS_SOCKET_ERR == EINTR;
#endif // !_WIN32
      if (!shouldRetry) {
        break;
      }
      threadYield(); // Yield between retries.
    }

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return '\0'; // Aborted.
    }

    if (bytesRead < 0) {
      if (errno == EAGAIN) {
        *pErr = PlatformError::TcpTimeout;
      } else {
        *pErr = getTcpPlatformError();
      }
      m_state.store(TcpStreamState::Failed, std::memory_order_release);
      return '\0';
    }
    if (bytesRead == 0) {
      *pErr = PlatformError::StreamNoDataAvailable;
      return '\0';
    }
    return res;
  }

  auto writeString(ExecutorHandle* execHandle, PlatformError* pErr, StringRef* str) noexcept
      -> bool {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      *pErr = PlatformError::TcpInvalidConnectionSocket;
      return false;
    }
    if (str->getSize() == 0) {
      return true;
    }

    execHandle->setState(ExecState::Paused);

    int bytesWritten = -1;
    while (m_state.load(std::memory_order_acquire) == TcpStreamState::Valid) {
      bytesWritten = ::send(m_socket, str->getCharDataPtr(), str->getSize(), 0);
      if (bytesWritten >= 0) {
        break; // No error while writing.
      }

      // Retry the send for certain errors.
#if defined(_WIN32)
      const bool shouldRetry = OS_SOCKET_ERR == WSAEINTR;
#else // !_WIN32
      const bool shouldRetry = OS_SOCKET_ERR == EINTR;
#endif // !_WIN32
      if (!shouldRetry) {
        break;
      }
      threadYield(); // Yield between retries.
    }

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false; // Aborted.
    }

    if (bytesWritten < 0) {
      *pErr = getTcpPlatformError();
      m_state.store(TcpStreamState::Failed, std::memory_order_release);
      return false;
    }
    if (bytesWritten != static_cast<int>(str->getSize())) {
      *pErr = PlatformError::TcpUnknownError;
      return false;
    }
    return true;
  }

  auto writeChar(ExecutorHandle* execHandle, PlatformError* pErr, uint8_t val) noexcept -> bool {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      *pErr = PlatformError::TcpInvalidConnectionSocket;
      return false;
    }

    execHandle->setState(ExecState::Paused);

    auto* valChar    = static_cast<char*>(static_cast<void*>(&val));
    int bytesWritten = -1;
    while (m_state.load(std::memory_order_acquire) == TcpStreamState::Valid) {
      bytesWritten = ::send(m_socket, valChar, 1, 0);
      if (bytesWritten >= 0) {
        break; // No error while writing.
      }

      // Retry the send for certain errors.
#if defined(_WIN32)
      const bool shouldRetry = OS_SOCKET_ERR == WSAEINTR;
#else // !_WIN32
      const bool shouldRetry = OS_SOCKET_ERR == EINTR;
#endif // !_WIN32
      if (!shouldRetry) {
        break;
      }
      threadYield(); // Yield between retries.
    }

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    if (bytesWritten < 0) {
      *pErr = getTcpPlatformError();
      m_state.store(TcpStreamState::Failed, std::memory_order_release);
      return false;
    }
    if (bytesWritten == 0) {
      *pErr = PlatformError::TcpUnknownError;
      return false;
    }
    return true;
  }

  auto flush(PlatformError* /*unused*/) noexcept -> bool {
    // Is a no-op at the moment, in the future we can considering asking the os to actually send the
    // data. Perhaps by temporary disabling the nagle algorithm (TCP_NODELAY)?
    return true;
  }

  auto setOpts(PlatformError* pErr, StreamOpts /*unused*/) noexcept -> bool {
    // TODO(bastian): Support non-blocking sockets.
    *pErr = PlatformError::StreamOptionsNotSupported;
    return false;
  }

  auto unsetOpts(PlatformError* pErr, StreamOpts /*unused*/) noexcept -> bool {
    // TODO(bastian): Support non-blocking sockets.
    *pErr = PlatformError::StreamOptionsNotSupported;
    return false;
  }

  auto acceptConnection(ExecutorHandle* execHandle, RefAllocator* alloc, PlatformError* pErr)
      -> TcpStreamRef* {
    if (unlikely(m_type != TcpStreamType::Server)) {
      *pErr = PlatformError::TcpInvalidServerSocket;
      return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, OS_INVALID_SOCKET);
    }

    // 'accept' call blocks so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    OS_SOCKET sock = OS_INVALID_SOCKET;
    while (m_state.load(std::memory_order_acquire) == TcpStreamState::Valid) {
      // Accept a new connection from the socket.
      sock = ::accept(m_socket, nullptr, nullptr);
      if (OS_SOCKET_VALID(sock)) {
        break; // Valid connection accepted.
      }

      // Retry the accept for certain errors.
#if defined(_WIN32)
      const bool shouldRetry = OS_SOCKET_ERR == WSAEINTR || OS_SOCKET_ERR == WSAEWOULDBLOCK ||
          OS_SOCKET_ERR == WSAECONNRESET;
#else // !_WIN32
      const bool shouldRetry = OS_SOCKET_ERR == EINTR || OS_SOCKET_ERR == EAGAIN ||
          OS_SOCKET_ERR == EWOULDBLOCK || OS_SOCKET_ERR == ECONNABORTED;
#endif // !_WIN32
      if (!shouldRetry) {
        break;
      }
      threadYield(); // Yield between retries.
    }

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return nullptr; // Aborted.
    }

    if (!OS_SOCKET_VALID(sock)) {
      *pErr = getTcpPlatformError();
      return alloc->allocPlain<TcpStreamRef>(
          TcpStreamType::Connection, sock, TcpStreamState::Failed);
    }

    configureSocket(sock);
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock);
  }

private:
  TcpStreamType m_type;
  std::atomic<TcpStreamState> m_state;
  OS_SOCKET m_socket;

  inline explicit TcpStreamRef(TcpStreamType type, OS_SOCKET sock) noexcept :
      Ref{getKind()}, m_type{type}, m_state{TcpStreamState::Valid}, m_socket{sock} {}

  inline TcpStreamRef(TcpStreamType type, OS_SOCKET sock, TcpStreamState state) noexcept :
      Ref{getKind()}, m_type{type}, m_state{state}, m_socket{sock} {}
};

inline auto configureSocket(OS_SOCKET sock) noexcept -> void {
  // Allow reusing the address, allows stopping and restarting the server without waiting for the
  // socket's wait-time to expire.
  int optVal = 1;
  ::setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&optVal), sizeof(int));

  // Configure the receive timeout;
#if defined(_WIN32)
  int timeout = receiveTimeoutSeconds * 1'000;
  ::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
#else // !_WIN32
  auto timeout = timeval{};
  timeout.tv_sec = receiveTimeoutSeconds;
  timeout.tv_usec = 0;
  ::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
#endif // !_WIN32
}

inline auto tcpOpenConnection(
    const Settings* settings,
    ExecutorHandle* execHandle,
    RefAllocator* alloc,
    PlatformError* pErr,
    StringRef* address,
    IpAddressFamily family,
    int32_t port) noexcept -> TcpStreamRef* {

  if (!settings->socketsEnabled) {
    *pErr = PlatformError::FeatureNetworkNotEnabled;
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, OS_INVALID_SOCKET);
  }

  if (port < 0 || port > std::numeric_limits<uint16_t>::max()) {
    *pErr = PlatformError::TcpInvalidPort;
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, OS_INVALID_SOCKET);
  }

  if (!isIpFamilyValid(family)) {
    *pErr = PlatformError::TcpInvalidAddressFamily;
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, OS_INVALID_SOCKET);
  }

  if (address->getSize() != getIpAddressSize(family)) {
    *pErr = PlatformError::TcpInvalidAddress;
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, OS_INVALID_SOCKET);
  }

  // Open a socket.
  const auto sock = socket(getIpFamilyCode(family), SOCK_STREAM, 0);
  if (!OS_SOCKET_VALID(sock)) {
    *pErr = getTcpPlatformError();
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock, TcpStreamState::Failed);
  }

  configureSocket(sock);

  // 'connect' call blocks so we mark ourselves as paused so the gc can trigger in the mean time.
  execHandle->setState(ExecState::Paused);

  int res = -1;
  while (true) {
    switch (family) {
    case IpAddressFamily::IpV4: {
      sockaddr_in addr = {};
      addr.sin_family  = AF_INET;
      addr.sin_port    = htons(static_cast<uint16_t>(port));
      std::memcpy(&addr.sin_addr, address->getCharDataPtr(), 4u);
      res = ::connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));
    } break;
    case IpAddressFamily::IpV6: {
      sockaddr_in6 addr = {};
      addr.sin6_family  = AF_INET6;
      addr.sin6_port    = htons(static_cast<uint16_t>(port));
      std::memcpy(&addr.sin6_addr, address->getCharDataPtr(), 16u);
      res = ::connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in6));
    } break;
    }
    if (res == 0) {
      break; // Success.
    }
#if defined(_WIN32)
    const bool shouldRetry = OS_SOCKET_ERR == WSAEINTR;
#else // !_WIN32
    const bool shouldRetry = OS_SOCKET_ERR == EINTR || OS_SOCKET_ERR == EAGAIN;
#endif // !_WIN32
    if (!shouldRetry) {
      break; // Not an error we should retry.
    }
    threadYield(); // Yield between retries.
  }

  // After resuming check if we should wait for gc (or if we are aborted).
  execHandle->setState(ExecState::Running);
  if (execHandle->trap()) {
    return nullptr;
  }

  if (res < 0) {
    *pErr = getTcpPlatformError();
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock, TcpStreamState::Failed);
  }

  // Socket has successfully connected to the remote.
  return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock);
}

inline auto tcpStartServer(
    const Settings* settings,
    RefAllocator* alloc,
    PlatformError* pErr,
    IpAddressFamily family,
    int32_t port,
    int32_t backlog) noexcept -> TcpStreamRef* {

  if (!settings->socketsEnabled) {
    *pErr = PlatformError::FeatureNetworkNotEnabled;
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, OS_INVALID_SOCKET);
  }

  if (port < 0 || port > std::numeric_limits<uint16_t>::max()) {
    *pErr = PlatformError::TcpInvalidPort;
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, OS_INVALID_SOCKET);
  }

  if (backlog > std::numeric_limits<int16_t>::max()) {
    *pErr = PlatformError::TcpInvalidBacklog;
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, OS_INVALID_SOCKET);
  }

  if (!isIpFamilyValid(family)) {
    *pErr = PlatformError::TcpInvalidAddressFamily;
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, OS_INVALID_SOCKET);
  }

  // Open a socket.
  const auto sock = ::socket(getIpFamilyCode(family), SOCK_STREAM, 0);
  if (!OS_SOCKET_VALID(sock)) {
    *pErr = PlatformError::TcpSocketCouldNotBeAllocated;
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock, TcpStreamState::Failed);
  }

  configureSocket(sock);

  // Bind the socket to any ip address at the given port.
  int res = -1;
  switch (family) {
  case IpAddressFamily::IpV4: {
    sockaddr_in addr     = {};
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(static_cast<uint16_t>(port));
    addr.sin_addr.s_addr = INADDR_ANY;
    res                  = ::bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));
  } break;
  case IpAddressFamily::IpV6: {
    sockaddr_in6 addr = {};
    addr.sin6_family  = AF_INET6;
    addr.sin6_port    = htons(static_cast<uint16_t>(port));
    addr.sin6_addr    = in6addr_any;
    res               = ::bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in6));
  } break;
  }

  if (res < 0) {
    *pErr = getTcpPlatformError();
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock, TcpStreamState::Failed);
  }

  // Start listening for new connections.
  if (listen(sock, backlog < 0 ? defaultConnectionBacklog : backlog) < 0) {
    *pErr = getTcpPlatformError();
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock, TcpStreamState::Failed);
  }

  // Socket is now ready to accept connections.
  return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock);
}

inline auto tcpAcceptConnection(
    ExecutorHandle* execHandle, RefAllocator* alloc, PlatformError* pErr, Value stream) noexcept
    -> TcpStreamRef* {

  auto* streamRef = stream.getRef();
  if (streamRef->getKind() != RefKind::StreamTcp) {
    *pErr = PlatformError::TcpInvalidSocket;
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, OS_INVALID_SOCKET);
  }
  auto* tcpStreamRef = static_cast<TcpStreamRef*>(streamRef);
  if (!tcpStreamRef->isValid()) {
    *pErr = PlatformError::TcpInvalidSocket;
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, OS_INVALID_SOCKET);
  }
  return tcpStreamRef->acceptConnection(execHandle, alloc, pErr);
}

inline auto tcpShutdown(PlatformError* pErr, Value stream) noexcept -> bool {

  auto* streamRef = stream.getRef();
  if (streamRef->getKind() != RefKind::StreamTcp) {
    *pErr = PlatformError::TcpInvalidSocket;
    return false;
  }
  auto* tcpStreamRef = static_cast<TcpStreamRef*>(streamRef);
  if (!tcpStreamRef->isValid()) {
    *pErr = PlatformError::TcpInvalidSocket;
    return false;
  }
  return tcpStreamRef->shutdown();
}

inline auto ipLookupAddress(
    const Settings* settings,
    ExecutorHandle* execHandle,
    RefAllocator* alloc,
    PlatformError* pErr,
    StringRef* hostName,
    IpAddressFamily family) noexcept -> StringRef* {

  if (!settings->socketsEnabled) {
    *pErr = PlatformError::FeatureNetworkNotEnabled;
    return alloc->allocStr(0);
  }

  if (!isIpFamilyValid(family)) {
    *pErr = PlatformError::TcpInvalidAddressFamily;
    return alloc->allocStr(0);
  }

  addrinfo hints    = {};
  hints.ai_family   = getIpFamilyCode(family);
  hints.ai_socktype = SOCK_STREAM;

  // 'getaddrinfo' call blocks so mark ourselves as paused so the gc can trigger in the mean time.
  execHandle->setState(ExecState::Paused);

  addrinfo* res = nullptr;
  auto resCode  = ::getaddrinfo(hostName->getCharDataPtr(), nullptr, &hints, &res);

  // After resuming check if we should wait for gc (or if we are aborted).
  execHandle->setState(ExecState::Running);
  if (execHandle->trap()) {
    // Cleanup the addinfo's incase any where allocated.
    if (res) {
      ::freeaddrinfo(res);
    }
    return nullptr; // Aborted.
  }

  if (resCode != 0 || !res) {
    switch (resCode) {
    case EAI_FAMILY:
    case EAI_FAIL:
    case EAI_NONAME:
      *pErr = PlatformError::TcpAddressNotFound;
      break;
    default:
      *pErr = PlatformError::TcpUnknownError;
      break;
    }
    return alloc->allocStr(0);
  }

  // Note: 'getaddrinfo' returns a linked list of addresses sorted by priority, we return the
  // first from the requested family.
  do {
    if (res->ai_family == hints.ai_family) {
      void* addrData = nullptr;
      switch (family) {
      case IpAddressFamily::IpV4:
        addrData = &reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr;
        break;
      case IpAddressFamily::IpV6:
        addrData = &reinterpret_cast<sockaddr_in6*>(res->ai_addr)->sin6_addr;
        break;
      }
      auto* str = alloc->allocStr(getIpAddressSize(family));
      std::memcpy(str->getCharDataPtr(), addrData, str->getSize());

      ::freeaddrinfo(res); // Free the list of addresses received from 'getaddrinfo'.
      return str;
    }
  } while ((res = res->ai_next));

  *pErr = PlatformError::TcpAddressNotFound;
  ::freeaddrinfo(res); // Free the list of addresses received from 'getaddrinfo'.
  return alloc->allocStr(0);
}

inline auto getTcpPlatformError() noexcept -> PlatformError {
#if defined(_WIN32)
  switch (OS_SOCKET_ERR) {
  case WSAEINPROGRESS:
    return PlatformError::TcpAlreadyInProcess;
  case WSAENETDOWN:
    return PlatformError::TcpNetworkDown;
  case WSAEMFILE:
  case WSAENOBUFS:
    return PlatformError::TcpSocketCouldNotBeAllocated;
  case WSAEACCES:
    return PlatformError::TcpNoAccess;
  case WSAEADDRINUSE:
    return PlatformError::TcpAddressInUse;
  case WSAEAFNOSUPPORT:
    return PlatformError::TcpAddressFamilyNotSupported;
  case WSAECONNREFUSED:
    return PlatformError::TcpConnectionRefused;
  case WSAETIMEDOUT:
    return PlatformError::TcpTimeout;
  case WSAEHOSTUNREACH:
    return PlatformError::TcpNetworkUnreachable;
  case WSAHOST_NOT_FOUND:
    return PlatformError::TcpAddressNotFound;
  case WSAENETRESET:
  case WSAECONNRESET:
    return PlatformError::TcpRemoteResetConnection;
  case WSAESHUTDOWN:
  case WSAENOTSOCK:
    return PlatformError::TcpSocketIsDead;
  }
#else // !_WIN32
  switch (OS_SOCKET_ERR) {
  case EALREADY:
    return PlatformError::TcpAlreadyInProcess;
  case ENETDOWN:
  case EPROTO:
  case EHOSTDOWN:
    return PlatformError::TcpNetworkDown;
  case ENETUNREACH:
    return PlatformError::TcpNetworkUnreachable;
  case EMFILE:
  case ENFILE:
  case ENOBUFS:
  case ENOMEM:
    return PlatformError::TcpSocketCouldNotBeAllocated;
  case EACCES:
    return PlatformError::TcpNoAccess;
  case EADDRINUSE:
    return PlatformError::TcpAddressInUse;
  case EADDRNOTAVAIL:
    return PlatformError::TcpAddressUnavailable;
  case EAFNOSUPPORT:
    return PlatformError::TcpAddressFamilyNotSupported;
  case ECONNREFUSED:
    return PlatformError::TcpConnectionRefused;
  case ETIMEDOUT:
    return PlatformError::TcpTimeout;
  case ECONNRESET:
    return PlatformError::TcpRemoteResetConnection;
  case EPIPE:
  case EPROTOTYPE:
  case EBADF:
  case EINVAL:
    return PlatformError::TcpSocketIsDead;
  }
#endif // !_WIN32
  return PlatformError::TcpUnknownError;
}

} // namespace vm::internal
