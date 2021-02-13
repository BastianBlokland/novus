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

  auto readChar(ExecutorHandle* execHandle, PlatformError* pErr) noexcept -> char {
    if (unlikely(m_kind != ConsoleStreamKind::StdIn)) {
      *pErr = PlatformError::StreamReadNotSupported;
      return false;
    }

    execHandle->setState(ExecState::Paused);

    char res;
    int bytesRead = 0;
#if defined(_WIN32)
    // TODO: Refactor this to use ReadConsoleInput for non-blocking input on windows.
    if (m_nonblockWinTerm) {
      if (_kbhit()) {
        res       = static_cast<char>(_getch());
        bytesRead = 1;
      }
    } else {
      bytesRead = fileRead(m_consoleHandle, &res, 1);
    }
#else //!_WIN32
    bytesRead = fileRead(m_consoleHandle, &res, 1);
#endif //!_WIN32

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return '\0'; // Aborted.
    }

    if (bytesRead != 1) {
      *pErr = getConsolePlatformError();
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
      return false;
    }

    if (bytesWritten != static_cast<int>(str->getSize())) {
      *pErr = getConsolePlatformError();
      return false;
    }
    return true;
  }

  auto writeChar(ExecutorHandle* execHandle, PlatformError* pErr, uint8_t val) noexcept -> bool {
    if (unlikely(m_kind == ConsoleStreamKind::StdIn)) {
      *pErr = PlatformError::StreamWriteNotSupported;
      return false;
    }

    execHandle->setState(ExecState::Paused);

    auto* valChar          = static_cast<char*>(static_cast<void*>(&val));
    const int bytesWritten = fileWrite(m_consoleHandle, valChar, 1);

    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    if (bytesWritten != 1) {
      *pErr = getConsolePlatformError();
      return false;
    }
    return true;
  }

  auto flush(PlatformError* /*unused*/) noexcept -> bool {
    // At the moment this is a no-op, in the future we can consider adding additional buffering to
    // console streams (so flush would write to the handle).
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
#else //!_WIN32
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
#else //!_WIN32
    if (unsetFileDescriptorOpts(m_consoleHandle, opts)) {
      return true;
    }
#endif //!_WIN32
    *pErr = PlatformError::StreamOptionsNotSupported;
    return false;
  }

private:
#if defined(_WIN32)
  bool m_nonblockWinTerm = false;
#endif
  FileHandle m_consoleHandle;
  ConsoleStreamKind m_kind;

  inline explicit ConsoleStreamRef(FileHandle con, ConsoleStreamKind kind) noexcept :
      Ref{getKind()}, m_consoleHandle{con}, m_kind{kind} {}

  [[nodiscard]] auto isTerminal() const noexcept -> bool {
#if defined(_WIN32)
    return GetFileType(m_consoleHandle) == FILE_TYPE_CHAR;
#else // !_WIN32
    return isatty(m_consoleHandle) == 1;
#endif
  }
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
  return PlatformError::ConsoleUnknownError;
}

} // namespace vm::internal
