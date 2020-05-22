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

enum class TcpStreamType : uint8_t {
  Server     = 0, // Server cannot be used for sending or receiving but can accept new connections.
  Connection = 1, // Connections are be used for sending and receiving.
};

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
      if (shutdown(m_socket, SD_BOTH) == 0) {
        closesocket(m_socket);
      }
#else  // !_WIN32
      if (shutdown(m_socket, SHUT_RDWR) == 0) {
        close(m_socket);
      }
#endif // !_WIN32
    }
  }

  auto operator=(const TcpStreamRef& rhs) -> TcpStreamRef& = delete;
  auto operator=(TcpStreamRef&& rhs) -> TcpStreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::StreamTcp; }

  [[nodiscard]] auto isValid() noexcept -> bool { return SOCKET_VALID(m_socket) && m_err == -1; }

  auto readString(RefAllocator* alloc, int32_t max) noexcept -> StringRef* {
    if (unlikely(max < 0 || m_type != TcpStreamType::Connection)) {
      return alloc->allocStr(0).first;
    }

    auto strAlloc   = alloc->allocStr(max); // allocStr already does +1 for null-ter.
    auto* tgtBuffer = static_cast<char*>(static_cast<void*>(strAlloc.second));
    auto bytesRead  = recv(m_socket, tgtBuffer, max, 0);
    if (bytesRead < 0) {
      m_err = SOCKET_ERR;
      return alloc->allocStr(0).first;
    }

    strAlloc.second[bytesRead] = '\0'; // null-terminate.
    strAlloc.first->updateSize(bytesRead);
    return strAlloc.first;
  }

  auto readChar() noexcept -> char {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      return '\0';
    }

    char res       = '\0';
    auto bytesRead = recv(m_socket, &res, 1, 0);
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

  auto writeString(StringRef* str) noexcept -> bool {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      return false;
    }

    auto bytesWritten = send(m_socket, str->getCharDataPtr(), str->getSize(), 0);
    if (bytesWritten < 0) {
      m_err = SOCKET_ERR;
      return false;
    }
    return bytesWritten == static_cast<int>(str->getSize());
  }

  auto writeChar(uint8_t val) noexcept -> bool {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      return false;
    }

    auto* valChar     = static_cast<char*>(static_cast<void*>(&val));
    auto bytesWritten = send(m_socket, valChar, 1, 0);
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
    return true;
  }

  auto unsetOpts(StreamOpts /*unused*/) noexcept -> bool {
    // TODO(bastian): Support non-blocking sockets.
    return true;
  }

  auto acceptConnection(RefAllocator* alloc) -> TcpStreamRef* {
    if (unlikely(m_type != TcpStreamType::Server)) {
      return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET, -1);
    }

    // Accept a new connection from the socket.
    const auto sock = accept(m_socket, nullptr, nullptr);
    if (!SOCKET_VALID(sock)) {
      return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock, SOCKET_ERR);
    }
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

// MinGw headers are missing the 'inet_pton' function, we can wrap the included windows
// 'WSAStringToAddress' function.
#if defined(_WIN32) && defined(__MINGW32__)

static auto inet_pton(int af, const char* src, void* dst) -> int {
  auto sockAddr     = sockaddr_storage{};
  auto sockAddrPtr  = static_cast<sockaddr*>(static_cast<void*>(&sockAddr));
  auto sockAddrSize = static_cast<int>(sizeof(sockaddr_storage));

  if (WSAStringToAddress(const_cast<char*>(src), af, nullptr, sockAddrPtr, &sockAddrSize) == 0) {
    switch (af) {
    case AF_INET: {
      auto result = (static_cast<sockaddr_in*>(static_cast<void*>(&sockAddr)))->sin_addr;
      *static_cast<in_addr*>(dst) = result;
      return 1;
    }
    case AF_INET6: {
      auto result = (static_cast<sockaddr_in6*>(static_cast<void*>(&sockAddr)))->sin6_addr;
      *static_cast<in6_addr*>(dst) = result;
      return 1;
    }
    }
  }
  return 0;
}

#endif // _WIN32

inline auto
tcpOpenConnection(const Settings& settings, RefAllocator* alloc, StringRef* address, int32_t port)
    -> TcpStreamRef* {

  if (!settings.socketsEnabled) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET);
  }

  // Open a socket.
  const auto sock = socket(AF_INET, SOCK_STREAM, 0);
  if (!SOCKET_VALID(sock)) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock, SOCKET_ERR);
  }

  sockaddr_in addr = {};
  addr.sin_family  = AF_INET;
  addr.sin_port    = htons(static_cast<uint16_t>(port));

  // Convert the ip address to binary form.
  if (inet_pton(AF_INET, address->getCharDataPtr(), &addr.sin_addr) != 1) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock, SOCKET_ERR);
  }

  // Connect to the remote address.
  if (connect(sock, static_cast<sockaddr*>(static_cast<void*>(&addr)), sizeof(sockaddr_in)) < 0) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock, SOCKET_ERR);
  }

  // Socket has successfully connected to the remote.
  return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, sock);
}

inline auto
tcpStartServer(const Settings& settings, RefAllocator* alloc, int32_t port, int32_t backlog)
    -> TcpStreamRef* {

  if (!settings.socketsEnabled) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, INVALID_SOCKET);
  }

  // Open a socket.
  const auto sock = socket(AF_INET, SOCK_STREAM, 0);
  if (!SOCKET_VALID(sock)) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock, SOCKET_ERR);
  }

  // Allow reusing the address, allows stopping and restarting the server without waiting for the
  // socket's wait-time to expire.
  int optVal = 1;
  setsockopt(
      sock, SOL_SOCKET, SO_REUSEADDR, static_cast<char*>(static_cast<void*>(&optVal)), sizeof(int));

  // Bind the socket to any ip address at the given port.
  sockaddr_in addr;
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(static_cast<uint16_t>(port));
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sock, static_cast<sockaddr*>(static_cast<void*>(&addr)), sizeof(sockaddr_in)) < 0) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock, SOCKET_ERR);
  }

  // Start listening for new connections.
  if (listen(sock, backlog < 0 ? defaultConnectionBacklog : backlog) < 0) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock, SOCKET_ERR);
  }

  // Socket is now ready to accept connections.
  return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, sock);
}

inline auto tcpAcceptConnection(const Settings& settings, RefAllocator* alloc, Value stream)
    -> TcpStreamRef* {

  if (!settings.socketsEnabled) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET);
  }

  auto* streamRef = stream.getRef();
  if (streamRef->getKind() != RefKind::StreamTcp) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET);
  }

  auto* tcpStreamRef = static_cast<TcpStreamRef*>(streamRef);
  if (!tcpStreamRef->isValid()) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, INVALID_SOCKET);
  }

  return tcpStreamRef->acceptConnection(alloc);
}

} // namespace vm::internal
