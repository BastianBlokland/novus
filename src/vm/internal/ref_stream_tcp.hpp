#pragma once
#include "internal/fd_utilities.hpp"
#include "internal/os_include.hpp"
#include "internal/ref.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "internal/settings.hpp"
#include "likely.hpp"
#include <cerrno>
#include <cstring>

#if defined(_WIN32)

#define SOCKET_VALID(SOCK) (SOCK != INVALID_SOCKET)
#define SOCKET_ERR WSAGetLastError()

#else // !_WIN32

#define SOCKET int
#define SOCKET_VALID(SOCK) (SOCK >= 0)
#define INVALID_SOCKET -1
#define SOCKET_ERR errno

#endif // !_WIN32

namespace vm::internal {

constexpr int32_t defaultConnectionBacklog = 64;
constexpr int32_t receiveTimeoutSeconds    = 15;

enum class TcpStreamType : uint8_t {
  Server     = 0, // Server cannot be used for sending or receiving but can accept new connections.
  Connection = 1, // Connections are be used for sending and receiving.
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

auto configureSocket(SOCKET sock) noexcept -> void;

// Tcp implementation of the 'stream' interface.
// Note: To avoid needing a vtable there is no abstract 'Stream' class but instead there are wrapper
// functions that dispatch based on the 'RefKind' (see stream_utilities.hpp).
class TcpStreamRef final : public Ref {
  friend class RefAllocator;

public:
  TcpStreamRef(const TcpStreamRef& rhs) = delete;
  TcpStreamRef(TcpStreamRef&& rhs)      = delete;
  ~TcpStreamRef() noexcept {
    if (SOCKET_VALID(m_socket)) {
#if defined(_WIN32)
      closesocket(m_socket);
#else  // !_WIN32
      close(m_socket);
#endif // !_WIN32
    }
  }

  auto operator=(const TcpStreamRef& rhs) -> TcpStreamRef& = delete;
  auto operator=(TcpStreamRef&& rhs) -> TcpStreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::StreamTcp; }

  [[nodiscard]] auto isValid() noexcept -> bool { return SOCKET_VALID(m_socket) && m_err == -1; }

  auto shutdown() noexcept -> bool {
#if defined(_WIN32)
    return ::shutdown(m_socket, SD_BOTH) == 0;
#else  // !_WIN32
    return ::shutdown(m_socket, SHUT_RDWR) == 0;
#endif // !_WIN32
  }

  auto readString(ExecutorHandle* execHandle, StringRef* str) noexcept -> bool {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      str->updateSize(0);
      return false;
    }
    if (unlikely(str->getSize() == 0)) {
      return false;
    }

    // 'recv' call can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    auto bytesRead = recv(m_socket, str->getCharDataPtr(), str->getSize(), 0);

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    if (bytesRead < 0) {
      m_err = SOCKET_ERR;
      str->updateSize(0);
      return false;
    }

    str->updateSize(bytesRead);
    return bytesRead > 0;
  }

  auto readChar(ExecutorHandle* execHandle) noexcept -> char {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      return '\0';
    }

    // 'recv' call can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    char res       = '\0';
    auto bytesRead = recv(m_socket, &res, 1, 0);

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return '\0';
    }

    switch (bytesRead) {
    case -1:
      m_err = SOCKET_ERR;
      [[fallthrough]];
    case 0:
      return '\0';
    default:
      return res;
    }
  }

  auto writeString(ExecutorHandle* execHandle, StringRef* str) noexcept -> bool {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      return false;
    }

    // 'send' call can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    auto bytesWritten = send(m_socket, str->getCharDataPtr(), str->getSize(), 0);

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    if (bytesWritten < 0) {
      m_err = SOCKET_ERR;
      return false;
    }
    return bytesWritten == static_cast<int>(str->getSize());
  }

  auto writeChar(ExecutorHandle* execHandle, uint8_t val) noexcept -> bool {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      return false;
    }

    // 'send' call can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    auto* valChar     = static_cast<char*>(static_cast<void*>(&val));
    auto bytesWritten = send(m_socket, valChar, 1, 0);

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    if (bytesWritten < 0) {
      m_err = SOCKET_ERR;
      return false;
    }
    return bytesWritten == 1;
  }

  auto flush() noexcept -> bool {
    // No way to 'flush' tcp sockets, we could consider temporary disabling the nagle algo
    // (TCP_NODELAY).
    return true;
  }

  auto setOpts(StreamOpts /*unused*/) noexcept -> bool {
    // TODO(bastian): Support non-blocking sockets.
    return false;
  }

  auto unsetOpts(StreamOpts /*unused*/) noexcept -> bool {
    // TODO(bastian): Support non-blocking sockets.
    return false;
  }

  auto acceptConnection(ExecutorHandle* execHandle, RefAllocator* alloc) -> TcpStreamRef* {
    if (unlikely(m_type != TcpStreamType::Server)) {
      return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET, -1);
    }

    // 'accept' call blocks so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    SOCKET sock;
    while (true) {
      // Accept a new connection from the socket.
      sock = accept(m_socket, nullptr, nullptr);

      // Retry the accept for certain errors.
      if (!SOCKET_VALID(sock)) {
#if defined(_WIN32)
        if (SOCKET_ERR == WSAEWOULDBLOCK || SOCKET_ERR == WSAECONNRESET) {
          continue; // Retry.
        }
#else  // !_WIN32
        if (SOCKET_ERR == EAGAIN || SOCKET_ERR == EWOULDBLOCK || SOCKET_ERR == ECONNABORTED) {
          continue; // Retry.
        }
#endif // !_WIN32
      }

      break; // Stop trying to accept a new connection.
    }

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return nullptr;
    }

    if (!SOCKET_VALID(sock)) {
      return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock, SOCKET_ERR);
    }

    configureSocket(sock);
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock);
  }

private:
  TcpStreamType m_type;
  SOCKET m_socket;
  int m_err;

  inline explicit TcpStreamRef(TcpStreamType type, SOCKET sock) noexcept :
      Ref{getKind()}, m_type{type}, m_socket{sock}, m_err{-1} {}

  inline TcpStreamRef(TcpStreamType type, SOCKET sock, int err) noexcept :
      Ref{getKind()}, m_type{type}, m_socket{sock}, m_err{err} {}
};

inline auto configureSocket(SOCKET sock) noexcept -> void {
  // Allow reusing the address, allows stopping and restarting the server without waiting for the
  // socket's wait-time to expire.
  int optVal = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&optVal), sizeof(int));

  // Configure the receive timeout;
#if defined(_WIN32)
  int timeout = receiveTimeoutSeconds * 1'000;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
#else  // !_WIN32
  auto timeout = timeval{};
  timeout.tv_sec = receiveTimeoutSeconds;
  timeout.tv_usec = 0;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
#endif // !_WIN32
}

inline auto tcpOpenConnection(
    const Settings& settings,
    ExecutorHandle* execHandle,
    RefAllocator* alloc,
    StringRef* address,
    IpAddressFamily family,
    int32_t port) noexcept -> TcpStreamRef* {

  if (!settings.socketsEnabled) {
    // Sockets are not enabled on this runtime.
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET);
  }

  if (port < 0 || port > std::numeric_limits<uint16_t>::max()) {
    // Invalid port number.
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET);
  }

  if (!isIpFamilyValid(family)) {
    // Invalid address family.
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET);
  }

  if (address->getSize() != getIpAddressSize(family)) {
    // Invalid address size.
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET);
  }

  // Open a socket.
  const auto sock = socket(getIpFamilyCode(family), SOCK_STREAM, 0);
  if (!SOCKET_VALID(sock)) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock, SOCKET_ERR);
  }

  configureSocket(sock);

  // 'connect' call blocks so we mark ourselves as paused so the gc can trigger in the mean time.
  execHandle->setState(ExecState::Paused);

  int res = -1;
  switch (family) {
  case IpAddressFamily::IpV4: {
    sockaddr_in addr = {};
    addr.sin_family  = AF_INET;
    addr.sin_port    = htons(static_cast<uint16_t>(port));
    std::memcpy(&addr.sin_addr, address->getCharDataPtr(), 4u);
    res = connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));
  } break;
  case IpAddressFamily::IpV6: {
    sockaddr_in6 addr = {};
    addr.sin6_family  = AF_INET6;
    addr.sin6_port    = htons(static_cast<uint16_t>(port));
    std::memcpy(&addr.sin6_addr, address->getCharDataPtr(), 16u);
    res = connect(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in6));
  } break;
  }

  // After resuming check if we should wait for gc (or if we are aborted).
  execHandle->setState(ExecState::Running);
  if (execHandle->trap()) {
    return nullptr;
  }

  if (res < 0) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock, SOCKET_ERR);
  }

  // Socket has successfully connected to the remote.
  return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock);
}

inline auto tcpStartServer(
    const Settings& settings,
    RefAllocator* alloc,
    IpAddressFamily family,
    int32_t port,
    int32_t backlog) noexcept -> TcpStreamRef* {

  if (!settings.socketsEnabled) {
    // Sockets are not enabled on this runtime.
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, INVALID_SOCKET);
  }

  if (port < 0 || port > std::numeric_limits<uint16_t>::max()) {
    // Invalid port number.
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, INVALID_SOCKET);
  }

  if (backlog > std::numeric_limits<int16_t>::max()) {
    // Invalid backlog.
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, INVALID_SOCKET);
  }

  if (!isIpFamilyValid(family)) {
    // Invalid address family.
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET);
  }

  // Open a socket.
  const auto sock = socket(getIpFamilyCode(family), SOCK_STREAM, 0);
  if (!SOCKET_VALID(sock)) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock, SOCKET_ERR);
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
    res                  = bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));
  } break;
  case IpAddressFamily::IpV6: {
    sockaddr_in6 addr = {};
    addr.sin6_family  = AF_INET6;
    addr.sin6_port    = htons(static_cast<uint16_t>(port));
    addr.sin6_addr    = in6addr_any;
    res               = bind(sock, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in6));
  } break;
  }

  if (res < 0) {
    // Failed to bind socket to port.
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock, SOCKET_ERR);
  }

  // Start listening for new connections.
  if (listen(sock, backlog < 0 ? defaultConnectionBacklog : backlog) < 0) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock, SOCKET_ERR);
  }

  // Socket is now ready to accept connections.
  return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock);
}

inline auto
tcpAcceptConnection(ExecutorHandle* execHandle, RefAllocator* alloc, Value stream) noexcept
    -> TcpStreamRef* {
  auto* streamRef = stream.getRef();
  if (streamRef->getKind() != RefKind::StreamTcp) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET);
  }
  auto* tcpStreamRef = static_cast<TcpStreamRef*>(streamRef);
  if (!tcpStreamRef->isValid()) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET);
  }
  return tcpStreamRef->acceptConnection(execHandle, alloc);
}

inline auto tcpShutdown(Value stream) noexcept -> bool {
  auto* streamRef = stream.getRef();
  if (streamRef->getKind() != RefKind::StreamTcp) {
    return false;
  }
  auto* tcpStreamRef = static_cast<TcpStreamRef*>(streamRef);
  if (!tcpStreamRef->isValid()) {
    return false;
  }
  return tcpStreamRef->shutdown();
}

inline auto ipLookupAddress(
    const Settings& settings,
    ExecutorHandle* execHandle,
    RefAllocator* alloc,
    StringRef* hostName,
    IpAddressFamily family) noexcept -> StringRef* {

  if (!settings.socketsEnabled) {
    // Sockets are not enabled on this runtime.
    return alloc->allocStr(0);
  }

  if (!isIpFamilyValid(family)) {
    // Invalid address family.
    return alloc->allocStr(0);
  }

  addrinfo hints    = {};
  hints.ai_family   = getIpFamilyCode(family);
  hints.ai_socktype = SOCK_STREAM;

  // 'getaddrinfo' call blocks so mark ourselves as paused so the gc can trigger in the mean time.
  execHandle->setState(ExecState::Paused);

  addrinfo* res = nullptr;
  auto resCode  = getaddrinfo(hostName->getCharDataPtr(), nullptr, &hints, &res);

  // After resuming check if we should wait for gc (or if we are aborted).
  execHandle->setState(ExecState::Running);
  if (execHandle->trap()) {

    // Cleanup the addinfo's incase any where allocated.
    if (res) {
      freeaddrinfo(res);
    }
    return nullptr;
  }

  if (resCode != 0 || !res) {
    return alloc->allocStr(0);
  }

  // Note: 'getaddrinfo' returns a linked list of addresses sorted by priority, we return the first
  // from the requested family.
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

      freeaddrinfo(res); // Free the list of addresses received from 'getaddrinfo'.
      return str;
    }
  } while ((res = res->ai_next));

  freeaddrinfo(res); // Free the list of addresses received from 'getaddrinfo'.
  return alloc->allocStr(0);
}

} // namespace vm::internal
