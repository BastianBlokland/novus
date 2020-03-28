#pragma once
#include "gsl.hpp"
#include "internal/ref_stream.hpp"
#include <cstdio>

namespace vm::internal {

enum class ConsoleStreamKind : uint8_t {
  StdIn  = 0,
  StdOut = 1,
  StdErr = 2,
};

class ConsoleStreamRef final : public StreamRef {
  friend class Allocator;

public:
  ConsoleStreamRef(const ConsoleStreamRef& rhs) = delete;
  ConsoleStreamRef(ConsoleStreamRef&& rhs)      = delete;
  ~ConsoleStreamRef() noexcept override         = default;

  auto operator=(const ConsoleStreamRef& rhs) -> ConsoleStreamRef& = delete;
  auto operator=(ConsoleStreamRef&& rhs) -> ConsoleStreamRef& = delete;

  [[nodiscard]] auto isValid() noexcept -> bool override { return m_filePtr != nullptr; }

  auto readString(Allocator* alloc, int32_t max) noexcept -> StringRef* override {
    auto strAlloc              = alloc->allocStr(max); // allocStr already does +1 for null-ter.
    auto bytesRead             = std::fread(strAlloc.second, 1U, max, m_filePtr);
    strAlloc.second[bytesRead] = '\0'; // null-terminate.
    strAlloc.first->updateSize(bytesRead);
    return strAlloc.first;
  }

  auto readChar() noexcept -> char override {
    auto res = std::getc(m_filePtr);
    return res > 0 ? static_cast<char>(res) : '\0';
  }

  auto writeString(StringRef* str) noexcept -> bool override {
    return std::fwrite(str->getDataPtr(), str->getSize(), 1, m_filePtr) == 1;
  }

  auto writeChar(uint8_t val) noexcept -> bool override {
    return std::fputc(val, m_filePtr) == val;
  }

  auto flush() noexcept -> void override { std::fflush(m_filePtr); }

private:
  FILE* m_filePtr;

  inline explicit ConsoleStreamRef(FILE* filePtr) noexcept : StreamRef{}, m_filePtr{filePtr} {}
};

inline auto openConsoleStream(Allocator* alloc, ConsoleStreamKind kind) -> StreamRef* {
  FILE* file;
  switch (kind) {
  case ConsoleStreamKind::StdIn:
    file = stdin;
    break;
  case ConsoleStreamKind::StdOut:
    file = stdout;
    break;
  case ConsoleStreamKind::StdErr:
    file = stderr;
    break;
  default:
    file = nullptr;
  }

  return alloc->allocPlain<ConsoleStreamRef>(file);
}

} // namespace vm::internal
