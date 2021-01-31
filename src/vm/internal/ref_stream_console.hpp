#pragma once
#include "gsl.hpp"
#include "internal/fd_utilities.hpp"
#include "internal/os_include.hpp"
#include "internal/ref.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "internal/terminal.hpp"
#include "intrinsics.hpp"
#include "vm/platform_interface.hpp"
#include <cerrno>
#include <cstdio>

namespace vm::internal {

enum class ConsoleStreamKind : uint8_t {
  StdIn  = 0,
  StdOut = 1,
  StdErr = 2,
};

// Console implementation of the 'stream' interface.
// Note: To avoid needing a vtable there is no abstract 'Stream' class but instead there are wrapper
// functions that dispatch based on the 'RefKind' (see stream_utilities.hpp).
class ConsoleStreamRef final : public Ref {
  friend class RefAllocator;

public:
  ConsoleStreamRef(const ConsoleStreamRef& rhs) = delete;
  ConsoleStreamRef(ConsoleStreamRef&& rhs)      = delete;
  ~ConsoleStreamRef() noexcept                  = default;

  auto operator=(const ConsoleStreamRef& rhs) -> ConsoleStreamRef& = delete;
  auto operator=(ConsoleStreamRef&& rhs) -> ConsoleStreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::StreamConsole; }

  [[nodiscard]] auto isValid() noexcept -> bool { return m_filePtr != nullptr; }

  auto readString(ExecutorHandle* execHandle, StringRef* str) noexcept -> bool {
    if (unlikely(str->getSize() == 0)) {
      return false;
    }

#if defined(_WIN32)
    // Special case non-blocking terminal read on windows. Unfortunately required as AFAIK there are
    // no non-blocking file-descriptors that can be used for terminal io.
    if (m_nonblockWinTerm) {
      auto size = 0U;
      while (size != str->getSize() && _kbhit()) {
        str->getCharDataPtr()[size] = static_cast<char>(_getch());
        size++;
      }
      str->updateSize(size);
      return size > 0;
    }
#endif

    // Can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    auto bytesRead = std::fread(str->getCharDataPtr(), 1U, str->getSize(), m_filePtr);

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    str->updateSize(bytesRead);
    return bytesRead > 0;
  }

  auto readChar(ExecutorHandle* execHandle) noexcept -> char {
#if defined(_WIN32)
    // Special case non-blocking terminal read on windows. Unfortunately required as AFAIK there are
    // no non-blocking file-descriptors that can be used for terminal io.
    if (m_nonblockWinTerm) {
      auto res = _kbhit() ? static_cast<char>(_getch()) : '\0';
      return res > 0 ? res : '\0';
    }
#endif

    // Can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    const auto res = std::getc(m_filePtr);

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return '\0';
    }

    return res > 0 ? static_cast<char>(res) : '\0';
  }

  auto writeString(ExecutorHandle* execHandle, StringRef* str) noexcept -> bool {

    // Can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    size_t itemsWritten;
    do {
      itemsWritten = std::fwrite(str->getDataPtr(), str->getSize(), 1, m_filePtr);
    } while (itemsWritten != 1u && errno == EAGAIN);

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    return itemsWritten == 1u;
  }

  auto writeChar(ExecutorHandle* execHandle, uint8_t val) noexcept -> bool {

    // Can block so we mark ourselves as paused so the gc can trigger in the mean time.
    execHandle->setState(ExecState::Paused);

    int charWritten;
    do {
      charWritten = std::fputc(val, m_filePtr);
    } while (charWritten != val && errno == EAGAIN);

    // After resuming check if we should wait for gc (or if we are aborted).
    execHandle->setState(ExecState::Running);
    if (execHandle->trap()) {
      return false;
    }

    return charWritten == val;
  }

  auto flush() noexcept -> bool {
    if (std::fflush(m_filePtr) != 0) {
#if !defined(_WIN32)
      if (errno == EAGAIN) {
        return true;
      }
#endif // !_WIN32
      return false;
    }
    return true;
  }

  auto setOpts(StreamOpts opts) noexcept -> bool {
#if defined(_WIN32)
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(StreamOpts::NoBlock)) {
      if (!hasTerminal()) {
        // If the console stream is not a terminal (but a pipe for example) we cannot
        // (easily) support non-blocking reads on windows.
        return false;
      }
      m_nonblockWinTerm = true;
      return true;
    }
    return false;
#else //!_WIN32
    return setFileOpts(m_filePtr, opts);
#endif
  }

  auto unsetOpts(StreamOpts opts) noexcept -> bool {
#if defined(_WIN32)
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(StreamOpts::NoBlock)) {
      m_nonblockWinTerm = false;
      return true;
    }
    return false;
#else //!_WIN32
    return unsetFileOpts(m_filePtr, opts);
#endif
  }

private:
#if defined(_WIN32)
  bool m_nonblockWinTerm = false;
#endif
  FILE* m_filePtr;

  inline explicit ConsoleStreamRef(FILE* filePtr) noexcept : Ref{getKind()}, m_filePtr{filePtr} {}
};

inline auto openConsoleStream(PlatformInterface* iface, RefAllocator* alloc, ConsoleStreamKind kind)
    -> ConsoleStreamRef* {
  FILE* file;
  switch (kind) {
  case ConsoleStreamKind::StdIn:
    file = iface->getStdIn();
    break;
  case ConsoleStreamKind::StdOut:
    file = iface->getStdOut();
    break;
  case ConsoleStreamKind::StdErr:
    file = iface->getStdErr();
    break;
  default:
    file = nullptr;
  }

  return alloc->allocPlain<ConsoleStreamRef>(file);
}

} // namespace vm::internal
