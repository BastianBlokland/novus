#pragma once
#include "gsl.hpp"
#include "internal/allocator.hpp"
#include "internal/fd_utilities.hpp"
#include "internal/ref.hpp"
#include "internal/ref_string.hpp"
#include "internal/terminal.hpp"
#include "vm/platform_interface.hpp"
#include <cstdio>

#if defined(_WIN32)

#include <conio.h>

#endif

namespace vm::internal {

enum class ConsoleStreamKind : uint8_t {
  StdIn  = 0,
  StdOut = 1,
  StdErr = 2,
};

class ConsoleStreamRef final : public Ref {
  friend class Allocator;

public:
  ConsoleStreamRef(const ConsoleStreamRef& rhs) = delete;
  ConsoleStreamRef(ConsoleStreamRef&& rhs)      = delete;
  ~ConsoleStreamRef() noexcept                  = default;

  auto operator=(const ConsoleStreamRef& rhs) -> ConsoleStreamRef& = delete;
  auto operator=(ConsoleStreamRef&& rhs) -> ConsoleStreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::StreamConsole; }

  [[nodiscard]] auto isValid() noexcept -> bool { return m_filePtr != nullptr; }

  auto readString(Allocator* alloc, int32_t max) noexcept -> StringRef* {
#if defined(_WIN32)
    // Special case non-blocking terminal read on windows. Unfortunately required as AFAIK there are
    // no non-blocking file-descriptors that can be used for terminal io.
    if (m_nonblockWinTerm) {
      auto strAlloc = alloc->allocStr(max); // allocStr already does +1 for null-ter.
      auto size     = 0;
      while (size != max && _kbhit()) {
        strAlloc.second[size] = getch();
        size++;
      }
      strAlloc.second[size] = '\0'; // null-terminate.
      strAlloc.first->updateSize(size);
      return strAlloc.first;
    }
#endif

    auto strAlloc              = alloc->allocStr(max); // allocStr already does +1 for null-ter.
    auto bytesRead             = std::fread(strAlloc.second, 1U, max, m_filePtr);
    strAlloc.second[bytesRead] = '\0'; // null-terminate.
    strAlloc.first->updateSize(bytesRead);
    return strAlloc.first;
  }

  auto readChar() noexcept -> char {
#if defined(_WIN32)
    // Special case non-blocking terminal read on windows. Unfortunately required as AFAIK there are
    // no non-blocking file-descriptors that can be used for terminal io.
    if (m_nonblockWinTerm) {
      auto res = _kbhit() ? getch() : '\0';
      return res > 0 ? static_cast<char>(res) : '\0';
    }
#endif

    auto res = std::getc(m_filePtr);
    return res > 0 ? static_cast<char>(res) : '\0';
  }

  auto writeString(StringRef* str) noexcept -> bool {
    return std::fwrite(str->getDataPtr(), str->getSize(), 1, m_filePtr) == 1;
  }

  auto writeChar(uint8_t val) noexcept -> bool { return std::fputc(val, m_filePtr) == val; }

  auto flush() noexcept -> bool { return std::fflush(m_filePtr) == 0; }

  auto setOpts(StreamOpts opts) noexcept -> bool {
#if defined(_WIN32)
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(StreamOpts::NoBlock)) {
      if (!hasTerminal()) {
        // If the console stream is not a terminal (but a pipe for example) we cannot
        // (easily) support non-blocking reads on windows.
        return false;
      }
      m_nonblockWinTerm = true;
    }
    return true;
#else //!_WIN32
    return setFileOpts(m_filePtr, opts);
#endif
  }

  auto unsetOpts(StreamOpts opts) noexcept -> bool {
#if defined(_WIN32)
    if (static_cast<int32_t>(opts) & static_cast<int32_t>(StreamOpts::NoBlock)) {
      m_nonblockWinTerm = false;
    }
    return true;
#else //!_WIN32
    return unsetFileOpts(m_filePtr, opts);
#endif
  }

private:
  FILE* m_filePtr;
#if defined(_WIN32)
  bool m_nonblockWinTerm = false;
#endif

  inline explicit ConsoleStreamRef(FILE* filePtr) noexcept : Ref{getKind()}, m_filePtr{filePtr} {}
};

inline auto openConsoleStream(PlatformInterface* iface, Allocator* alloc, ConsoleStreamKind kind)
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
