#pragma once
#include "internal/ref.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_stream_console.hpp"
#include "internal/ref_stream_file.hpp"
#include "internal/ref_stream_process.hpp"
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
    case RefKind::StreamProcess:                                                                   \
      return downcastRef<ProcessStreamRef>(ref)->EXPR;                                             \
    default:                                                                                       \
      assert(false);                                                                               \
      return 0;                                                                                    \
    }                                                                                              \
  }

inline auto streamCheckValid(const Value& stream) noexcept -> bool {
  STREAM_DISPATCH(stream, isValid())
}

inline auto streamReadString(
    ExecutorHandle* execHandle, PlatformError* pErr, const Value& stream, StringRef* tgt) noexcept
    -> bool {

  if (!streamCheckValid(stream)) {
    tgt->updateSize(0);
    return false;
  }
  STREAM_DISPATCH(stream, readString(execHandle, pErr, tgt))
}

inline auto streamWriteString(
    ExecutorHandle* execHandle, PlatformError* pErr, const Value& stream, StringRef* str) noexcept
    -> bool {

  if (!streamCheckValid(stream)) {
    return false;
  }
  STREAM_DISPATCH(stream, writeString(execHandle, pErr, str))
}

inline auto streamSetOpts(PlatformError* pErr, const Value& stream, StreamOpts opts) noexcept
    -> bool {
  if (!streamCheckValid(stream)) {
    return false;
  }
  STREAM_DISPATCH(stream, setOpts(pErr, opts))
}

inline auto streamUnsetOpts(PlatformError* pErr, const Value& stream, StreamOpts opts) noexcept
    -> bool {
  if (!streamCheckValid(stream)) {
    return false;
  }
  STREAM_DISPATCH(stream, unsetOpts(pErr, opts))
}

#undef STREAM_DISPATCH

} // namespace vm::internal
