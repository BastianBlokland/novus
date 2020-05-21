#pragma once
#include "internal/fd_utilities.hpp"
#include "internal/ref.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "internal/settings.hpp"
#include "likely.hpp"
#include <cerrno>
#include <cstring>

#if !defined(_WIN32)

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

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
    if (m_socket >= 0) {
      close(m_socket);
    }
  }

  auto operator=(const TcpStreamRef& rhs) -> TcpStreamRef& = delete;
  auto operator=(TcpStreamRef&& rhs) -> TcpStreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::StreamTcp; }

  [[nodiscard]] auto isValid() noexcept -> bool { return m_socket != -1 && m_err == -1; }

  auto readString(RefAllocator* alloc, int32_t max) noexcept -> StringRef* {
    if (unlikely(max < 0 || m_type != TcpStreamType::Connection)) {
      return alloc->allocStr(0).first;
    }

    auto strAlloc  = alloc->allocStr(max); // allocStr already does +1 for null-ter.
    auto bytesRead = read(m_socket, strAlloc.second, max);
    if (bytesRead < 0) {
      m_err = errno;
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
    auto bytesRead = read(m_socket, &res, 1);
    switch (bytesRead) {
    case -1:
      m_err = errno;
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

    auto bytesWritten = write(m_socket, str->getCharDataPtr(), str->getSize());
    if (bytesWritten < 0) {
      m_err = errno;
      return false;
    }
    return bytesWritten == str->getSize();
  }

  auto writeChar(uint8_t val) noexcept -> bool {
    if (unlikely(m_type != TcpStreamType::Connection)) {
      return false;
    }

    auto bytesWritten = write(m_socket, &val, 1);
    if (bytesWritten < 0) {
      m_err = errno;
      return false;
    }
    return bytesWritten == 1;
  }

  auto flush() noexcept -> bool {
    // No way to 'flush' tcp sockets, we could consider temporary disabling the nagle algo
    // (TCP_NODELAY).
    return true;
  }

  auto setOpts(StreamOpts opts) noexcept -> bool { return setFileOpts(m_socket, opts); }

  auto unsetOpts(StreamOpts opts) noexcept -> bool { return unsetFileOpts(m_socket, opts); }

  auto acceptConnection(RefAllocator* alloc) -> TcpStreamRef* {
    if (unlikely(m_type != TcpStreamType::Server)) {
      return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, -1, -1);
    }

    // Accept a new connection from the socket.
    auto connFd = accept(m_socket, nullptr, nullptr);
    if (connFd < 0) {
      return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, connFd, errno);
    }
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, connFd);
  }

private:
  TcpStreamType m_type;
  int m_socket;
  int m_err;

  inline explicit TcpStreamRef(TcpStreamType type, int socket) noexcept :
      Ref{getKind()}, m_type{type}, m_socket{socket}, m_err{-1} {}

  inline TcpStreamRef(TcpStreamType type, int socket, int err) noexcept :
      Ref{getKind()}, m_type{type}, m_socket{socket}, m_err{err} {}
};

inline auto
tcpOpenConnection(const Settings& settings, RefAllocator* alloc, StringRef* address, int32_t port)
    -> TcpStreamRef* {

  if (!settings.socketsEnabled) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, -1);
  }

  // Open a socket.
  auto fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, fd, errno);
  }

  sockaddr_in endpointAddr = {};
  endpointAddr.sin_family  = AF_INET;
  endpointAddr.sin_port    = htons(static_cast<uint16_t>(port));

  // Convert the ip address to binary form.
  if (inet_pton(AF_INET, address->getCharDataPtr(), &endpointAddr.sin_addr) != 1) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, fd, errno);
  }

  // Connect to the remote endpoint.
  if (connect(fd, static_cast<sockaddr*>(static_cast<void*>(&endpointAddr)), sizeof(sockaddr_in)) <
      0) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, fd, errno);
  }

  // Socket has successfully connected to the remote.
  return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, fd);
}

inline auto
tcpStartServer(const Settings& settings, RefAllocator* alloc, int32_t port, int32_t backlog)
    -> TcpStreamRef* {

  if (!settings.socketsEnabled) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, -1);
  }

  // Open a socket.
  auto fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, fd, errno);
  }

  // Allow reusing the address, allows stopping and restarting the server without waiting for the
  // socket's wait-time to expire.
  int optVal = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, static_cast<void*>(&optVal), sizeof(int));

  // Bind the socket to any ip address at the given port.
  sockaddr_in addr;
  addr.sin_family      = AF_INET;
  addr.sin_port        = htons(static_cast<uint16_t>(port));
  addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(fd, static_cast<sockaddr*>(static_cast<void*>(&addr)), sizeof(sockaddr_in)) < 0) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, fd, errno);
  }

  // Start listening for new connections.
  if (listen(fd, backlog < 0 ? defaultConnectionBacklog : backlog) < 0) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, fd, errno);
  }

  // Socket is now ready to accept connections.
  return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Server, fd);
}

inline auto tcpAcceptConnection(const Settings& settings, RefAllocator* alloc, Value stream)
    -> TcpStreamRef* {

  if (!settings.socketsEnabled) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, -1);
  }

  auto* streamRef = stream.getRef();
  if (streamRef->getKind() != RefKind::StreamTcp) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, -1);
  }

  auto* tcpStreamRef = static_cast<TcpStreamRef*>(streamRef);
  if (!tcpStreamRef->isValid()) {
    return alloc->allocPlain<TcpStreamRef>(TcpStreamType::Connection, -1);
  }

  return tcpStreamRef->acceptConnection(alloc);
}

} // namespace vm::internal
