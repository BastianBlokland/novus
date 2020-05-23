#pragma once
#include "internal/ref.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_stream_console.hpp"
#include "internal/ref_stream_file.hpp"
#include "internal/ref_stream_tcp.hpp"
#include "internal/ref_string.hpp"
#include "internal/stream_opts.hpp"
#include "internal/value.hpp"

namespace vm::internal {

// Instead of making a virtual class we dispatch manually based on refKind. This avoids the size
// overhead of a vtable pointer.
#define STREAM_DISPATCH(STREAM, EXPR)                                                              \
  {                                                                                                \
    auto* ref = stream.getRef();                                                                   \
    switch (ref->getKind()) {                                                                      \
    case RefKind::StreamFile:                                                                      \
      return downcastRef<FileStreamRef>(ref)->EXPR;                                                \
    case RefKind::StreamConsole:                                                                   \
      return downcastRef<ConsoleStreamRef>(ref)->EXPR;                                             \
    case RefKind::StreamTcp:                                                                       \
      return downcastRef<TcpStreamRef>(ref)->EXPR;                                                 \
    default:                                                                                       \
      assert(false);                                                                               \
      return 0;                                                                                    \
    }                                                                                              \
  }

inline auto streamCheckValid(const Value& stream) noexcept -> bool {
  STREAM_DISPATCH(stream, isValid())
}

inline auto streamReadString(RefAllocator* alloc, const Value& stream, int max) noexcept
    -> StringRef* {
  if (!streamCheckValid(stream)) {
    return alloc->allocStr(0);
  }
  STREAM_DISPATCH(stream, readString(alloc, max))
}

inline auto streamReadChar(const Value& stream) noexcept -> char {
  if (!streamCheckValid(stream)) {
    return '\0';
  }
  STREAM_DISPATCH(stream, readChar())
}

inline auto streamWriteString(const Value& stream, StringRef* str) noexcept -> bool {
  if (!streamCheckValid(stream)) {
    return false;
  }
  STREAM_DISPATCH(stream, writeString(str))
}

inline auto streamWriteChar(const Value& stream, uint8_t val) noexcept -> bool {
  if (!streamCheckValid(stream)) {
    return false;
  }
  STREAM_DISPATCH(stream, writeChar(val))
}

inline auto streamFlush(const Value& stream) noexcept -> bool {
  if (!streamCheckValid(stream)) {
    return false;
  }
  STREAM_DISPATCH(stream, flush())
}

inline auto streamSetOpts(const Value& stream, StreamOpts opts) noexcept -> bool {
  if (!streamCheckValid(stream)) {
    return false;
  }
  STREAM_DISPATCH(stream, setOpts(opts))
}

inline auto streamUnsetOpts(const Value& stream, StreamOpts opts) noexcept -> bool {
  if (!streamCheckValid(stream)) {
    return false;
  }
  STREAM_DISPATCH(stream, unsetOpts(opts))
}

#undef STREAM_DISPATCH

} // namespace vm::internal
