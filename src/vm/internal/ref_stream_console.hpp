#pragma once
#include "gsl.hpp"
#include "internal/os_include.hpp"
#include "internal/platform_utilities.hpp"
#include "internal/ref.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "internal/stream_opts.hpp"
#include "internal/thread.hpp"
#include "intrinsics.hpp"
#include "vm/platform_interface.hpp"

namespace vm::internal {

enum class ConsoleStreamKind : uint8_t {
  StdIn  = 0,
  StdOut = 1,
  StdErr = 2,
};

enum class TermOpts : int32_t {
  NoEcho   = 1 << 0,
  NoBuffer = 1 << 1,
};

auto getConsolePlatformError() noexcept -> PlatformError;

#if !defined(_WIN32)
auto setFileDescriptorOpts(int fd, StreamOpts opts) noexcept -> bool;
auto unsetFileDescriptorOpts(int fd, StreamOpts opts) noexcept -> bool;
#endif // !_WIN32

// Console implementation of the 'stream' interface.
// Note: To avoid needing a vtable there is no abstract 'Stream' class but instead there are
// wrapper functions that dispatch based on the 'RefKind' (see stream_utilities.hpp).
class ConsoleStreamRef final : public Ref {
  friend class RefAllocator;

public:
  ConsoleStreamRef(const ConsoleStreamRef& rhs) = delete;
  ConsoleStreamRef(ConsoleStreamRef&& rhs)      = delete;
  ~ConsoleStreamRef() noexcept                  = default;

  auto operator=(const ConsoleStreamRef& rhs) -> ConsoleStreamRef& = delete;
  auto operator=(ConsoleStreamRef&& rhs) -> ConsoleStreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::StreamConsole; }

  [[nodiscard]] auto isValid() noexcept -> bool { return fileIsValid(m_consoleHandle); }

  auto readString(ExecutorHandle* execHandle, PlatformError* pErr, StringRef* str) noexcept
      -> bool {
    if (unlikely(m_kind != ConsoleStreamKind::StdIn)) {
      *pErr = PlatformError::StreamReadNotSupported;
      return false;
    }

    if (unlikely(str->getSize() == 0)) {
      return true;
    }

    execHandle->setState(ExecState::Paused);

    int bytesRead = 0;
#if defined(_WIN32)
    // TODO: Refactor this to use ReadConsoleInput for non-blocking input on windows.
    if (m_nonblockWinTerm) {
      while (bytesRead != static_cast<int>(str->getSize()) && _kbhit()) {
        str->getCharDataPtr()[bytesRead++] = static_cast<char>(_getch());
      }
    } else {
      bytesRead = fileRead(m_consoleHandle, str->getCharDataPtr(), str->getSize());
    }
#else //!_WIN32
    bytesRead = fileRead(m_consoleHandle, str->getCharDataPtr(), str->getSize());
#endif

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false; // Aborted.
    }

    if (bytesRead < 0) {
      *pErr = getConsolePlatformError();
      str->updateSize(0);
      return false;
    }
    if (bytesRead == 0) {
      *pErr = PlatformError::StreamNoDataAvailable;
      str->updateSize(0);
      return false;
    }
    str->updateSize(bytesRead);
    return bytesRead > 0;
  }

  auto writeString(ExecutorHandle* execHandle, PlatformError* pErr, StringRef* str) noexcept
      -> bool {
    if (unlikely(m_kind == ConsoleStreamKind::StdIn)) {
      *pErr = PlatformError::StreamWriteNotSupported;
      return false;
    }

    if (unlikely(str->getSize() == 0)) {
      return true;
    }

    execHandle->setState(ExecState::Paused);

    const int bytesWritten = fileWrite(m_consoleHandle, str->getCharDataPtr(), str->getSize());

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false; // Aborted.
    }

    if (bytesWritten != static_cast<int>(str->getSize())) {
      *pErr = getConsolePlatformError();
      return false;
    }
    return true;
  }

  auto setOpts(PlatformError* pErr, StreamOpts opts) noexcept -> bool {
#if defined(_WIN32)
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(StreamOpts::NoBlock)) {
      if (isTerminal()) {
        m_nonblockWinTerm = true;
        return true;
      }
    }
#else  //!_WIN32
    if (setFileDescriptorOpts(m_consoleHandle, opts)) {
      return true;
    }
#endif //!_WIN32
    *pErr = PlatformError::StreamOptionsNotSupported;
    return false;
  }

  auto unsetOpts(PlatformError* pErr, StreamOpts opts) noexcept -> bool {
#if defined(_WIN32)
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(StreamOpts::NoBlock)) {
      m_nonblockWinTerm = false;
      return true;
    }
#else  //!_WIN32
    if (unsetFileDescriptorOpts(m_consoleHandle, opts)) {
      return true;
    }
#endif //!_WIN32
    *pErr = PlatformError::StreamOptionsNotSupported;
    return false;
  }

  [[nodiscard]] auto isTerminal() const noexcept -> bool {
#if defined(_WIN32)
    return GetFileType(m_consoleHandle) == FILE_TYPE_CHAR;
#else // !_WIN32
    return isatty(m_consoleHandle) == 1;
#endif
  }

  [[nodiscard]] auto getTermWidth(PlatformError* pErr) const noexcept -> int32_t {
    if (!isTerminal()) {
      *pErr = PlatformError::ConsoleNoTerminal;
      return -1;
    }
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!::GetConsoleScreenBufferInfo(m_consoleHandle, &csbi)) {
      *pErr = PlatformError::ConsoleFailedToGetTermInfo;
      return -1;
    }
    return 1 + csbi.srWindow.Right - csbi.srWindow.Left;
#else // !_WIN32
    winsize ws;
    if (::ioctl(m_consoleHandle, TIOCGWINSZ, &ws)) {
      *pErr = PlatformError::ConsoleFailedToGetTermInfo;
      return -1;
    }
    return static_cast<int32_t>(ws.ws_col);
#endif
  }

  [[nodiscard]] auto getTermHeight(PlatformError* pErr) const noexcept -> int32_t {
    if (!isTerminal()) {
      *pErr = PlatformError::ConsoleNoTerminal;
      return -1;
    }
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (!::GetConsoleScreenBufferInfo(m_consoleHandle, &csbi)) {
      *pErr = PlatformError::ConsoleFailedToGetTermInfo;
      return -1;
    }
    return 1 + csbi.srWindow.Bottom - csbi.srWindow.Top;
#else // !_WIN32
    winsize ws;
    if (::ioctl(m_consoleHandle, TIOCGWINSZ, &ws)) {
      *pErr = PlatformError::ConsoleFailedToGetTermInfo;
      return -1;
    }
    return static_cast<int32_t>(ws.ws_row);
#endif
  }

  [[nodiscard]] auto setTermOpts(PlatformError* pErr, TermOpts opts) const noexcept -> bool {
    if (!isTerminal()) {
      *pErr = PlatformError::ConsoleNoTerminal;
      return false;
    }
#if defined(_WIN32)
    DWORD mode = 0;
    if (!::GetConsoleMode(m_consoleHandle, &mode)) {
      *pErr = PlatformError::ConsoleFailedToGetTermInfo;
      return false;
    }
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoEcho)) {
      mode &= ~ENABLE_ECHO_INPUT;
    }
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoBuffer)) {
      mode &= ~ENABLE_LINE_INPUT;
    }
    if (!::SetConsoleMode(m_consoleHandle, mode)) {
      *pErr = PlatformError::ConsoleFailedToUpdateTermInfo;
      return false;
    }
#else // !_WIN32
    termios t;
    if (::tcgetattr(m_consoleHandle, &t)) {
      *pErr = PlatformError::ConsoleFailedToGetTermInfo;
      return false;
    }
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoEcho)) {
      t.c_lflag &= ~ECHO;
    }
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoBuffer)) {
      t.c_lflag &= ~ICANON;
    }
    if (::tcsetattr(m_consoleHandle, TCSANOW, &t)) {
      *pErr = PlatformError::ConsoleFailedToUpdateTermInfo;
      return false;
    }
#endif
    return true;
  }

  [[nodiscard]] auto unsetTermOpts(PlatformError* pErr, TermOpts opts) const noexcept -> bool {
    if (!isTerminal()) {
      *pErr = PlatformError::ConsoleNoTerminal;
      return false;
    }
#if defined(_WIN32)
    DWORD mode = 0;
    if (!::GetConsoleMode(m_consoleHandle, &mode)) {
      *pErr = PlatformError::ConsoleFailedToGetTermInfo;
      return false;
    }
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoEcho)) {
      mode |= ENABLE_ECHO_INPUT;
    }
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoBuffer)) {
      mode |= ENABLE_LINE_INPUT;
    }
    if (!::SetConsoleMode(m_consoleHandle, mode)) {
      *pErr = PlatformError::ConsoleFailedToUpdateTermInfo;
      return false;
    }
#else // !_WIN32
    termios t;
    if (::tcgetattr(m_consoleHandle, &t)) {
      *pErr = PlatformError::ConsoleFailedToGetTermInfo;
      return false;
    }
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoEcho)) {
      t.c_lflag |= ECHO;
    }
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(TermOpts::NoBuffer)) {
      t.c_lflag |= ICANON;
    }
    if (::tcsetattr(m_consoleHandle, TCSANOW, &t)) {
      *pErr = PlatformError::ConsoleFailedToUpdateTermInfo;
      return false;
    }
#endif
    return true;
  }

private:
#if defined(_WIN32)
  bool m_nonblockWinTerm = false;
#endif
  FileHandle m_consoleHandle;
  ConsoleStreamKind m_kind;

  inline explicit ConsoleStreamRef(FileHandle con, ConsoleStreamKind kind) noexcept :
      Ref{getKind()}, m_consoleHandle{con}, m_kind{kind} {}
};
inline auto openConsoleStream(
    PlatformInterface* iface, RefAllocator* alloc, PlatformError* pErr, ConsoleStreamKind kind)
    -> ConsoleStreamRef* {

  FileHandle con = fileInvalid();
  switch (kind) {
  case ConsoleStreamKind::StdIn:
    con = iface->getStdIn();
    break;
  case ConsoleStreamKind::StdOut:
    con = iface->getStdOut();
    break;
  case ConsoleStreamKind::StdErr:
    con = iface->getStdErr();
    break;
  }

  if (!fileIsValid(con)) {
    *pErr = PlatformError::ConsoleNotPresent;
  }
  return alloc->allocPlain<ConsoleStreamRef>(con, kind);
}

inline auto getConsoleStream(PlatformError* pErr, Value stream) noexcept -> ConsoleStreamRef* {
  auto* streamRef = stream.getRef();
  if (streamRef->getKind() != RefKind::StreamConsole) {
    *pErr = PlatformError::ConsoleUnknownError;
    return nullptr;
  }
  auto* consoleStreamRef = static_cast<ConsoleStreamRef*>(streamRef);
  if (!consoleStreamRef->isValid()) {
    *pErr = PlatformError::ConsoleUnknownError;
    return nullptr;
  }
  return consoleStreamRef;
}

inline auto getIsTerm(PlatformError* pErr, Value stream) noexcept -> bool {
  auto* consoleStreamRef = getConsoleStream(pErr, stream);
  return consoleStreamRef && consoleStreamRef->isTerminal();
}

inline auto termGetWidth(PlatformError* pErr, Value stream) noexcept -> int32_t {
  auto* consoleStreamRef = getConsoleStream(pErr, stream);
  return consoleStreamRef ? consoleStreamRef->getTermWidth(pErr) : -1;
}

inline auto termGetHeight(PlatformError* pErr, Value stream) noexcept -> int32_t {
  auto* consoleStreamRef = getConsoleStream(pErr, stream);
  return consoleStreamRef ? consoleStreamRef->getTermHeight(pErr) : -1;
}

inline auto termSetOpts(PlatformError* pErr, Value stream, TermOpts opts) noexcept -> bool {
  auto* consoleStreamRef = getConsoleStream(pErr, stream);
  return consoleStreamRef && consoleStreamRef->setTermOpts(pErr, opts);
}

inline auto termUnsetOpts(PlatformError* pErr, Value stream, TermOpts opts) noexcept -> bool {
  auto* consoleStreamRef = getConsoleStream(pErr, stream);
  return consoleStreamRef && consoleStreamRef->unsetTermOpts(pErr, opts);
}

#if !defined(_WIN32)
inline auto setFileDescriptorOpts(int fd, StreamOpts opts) noexcept -> bool {
  int fileOpts = fcntl(fd, F_GETFL);
  if (fileOpts < 0) {
    return false;
  }
  auto updatedOptions = false;
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(StreamOpts::NoBlock)) {
    fileOpts |= O_NONBLOCK;
    updatedOptions = true;
  }
  if (fcntl(fd, F_SETFL, fileOpts) < 0) {
    return false;
  }
  return updatedOptions;
}

inline auto unsetFileDescriptorOpts(int fd, StreamOpts opts) noexcept -> bool {
  int fileOpts = fcntl(fd, F_GETFL);
  if (fileOpts < 0) {
    return false;
  }
  auto updatedOptions = false;
  if (static_cast<int32_t>(opts) & static_cast<int32_t>(StreamOpts::NoBlock)) {
    fileOpts &= ~O_NONBLOCK;
    updatedOptions = true;
  }
  if (fcntl(fd, F_SETFL, fileOpts) < 0) {
    return false;
  }
  return updatedOptions;
}
#endif // !_WIN32

inline auto getConsolePlatformError() noexcept -> PlatformError {
#if defined(_WIN32)

  switch (::GetLastError()) {
  case ERROR_BROKEN_PIPE:
    return PlatformError::StreamNoDataAvailable;
  }

#else // !_WIN32

  switch (errno) {
  case EAGAIN:
    return PlatformError::StreamNoDataAvailable;
  case ENOENT:
  case ETIMEDOUT:
  case EPIPE:
    return PlatformError::ConsoleNoLongerAvailable;
  }

#endif
  return PlatformError::ConsoleUnknownError;
}

} // namespace vm::internal
