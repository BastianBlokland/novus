#pragma once
#include "internal/allocator.hpp"
#include "internal/ref.hpp"
#include "internal/ref_string.hpp"
#include "internal/value.hpp"

namespace vm::internal {

enum class StreamOpts : int32_t { NoBlock = 1 << 0 };

class StreamRef : public Ref {
public:
  StreamRef(const StreamRef& rhs) = delete;
  StreamRef(StreamRef&& rhs)      = delete;
  ~StreamRef() noexcept override  = default;

  auto operator=(const StreamRef& rhs) -> StreamRef& = delete;
  auto operator=(StreamRef&& rhs) -> StreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::Stream; }

  [[nodiscard]] virtual auto isValid() noexcept -> bool = 0;

  virtual auto readString(Allocator* alloc, int32_t max) noexcept -> StringRef* = 0;
  virtual auto readChar() noexcept -> char                                      = 0;
  virtual auto writeString(StringRef* str) noexcept -> bool                     = 0;
  virtual auto writeChar(uint8_t val) noexcept -> bool                          = 0;
  virtual auto flush() noexcept -> void                                         = 0;
  virtual auto setOpts(StreamOpts opts) noexcept -> bool                        = 0;
  virtual auto unsetOpts(StreamOpts opts) noexcept -> bool                      = 0;

protected:
  inline StreamRef() noexcept : Ref(getKind()) {}
};

inline auto streamCheckValid(const Value& stream) noexcept -> bool {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  return streamRef->isValid();
}

inline auto streamReadString(Allocator* alloc, const Value& stream, int max) noexcept
    -> StringRef* {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  if (!streamRef->isValid()) {
    return alloc->allocStr(0).first;
  }
  return streamRef->readString(alloc, max);
}

inline auto streamReadChar(const Value& stream) noexcept -> char {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  if (!streamRef->isValid()) {
    return '\0';
  }
  return streamRef->readChar();
}

inline auto streamWriteString(const Value& stream, StringRef* str) noexcept -> bool {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  if (!streamRef->isValid()) {
    return false;
  }
  return streamRef->writeString(str);
}

inline auto streamWriteChar(const Value& stream, uint8_t val) noexcept -> bool {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  if (!streamRef->isValid()) {
    return false;
  }
  return streamRef->writeChar(val);
}

inline auto streamFlush(const Value& stream) noexcept -> void {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  if (streamRef->isValid()) {
    streamRef->flush();
  }
}

inline auto streamSetOpts(const Value& stream, StreamOpts opts) noexcept -> bool {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  if (!streamRef->isValid()) {
    return false;
  }
  return streamRef->setOpts(opts);
}

inline auto streamUnsetOpts(const Value& stream, StreamOpts opts) noexcept -> bool {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  if (!streamRef->isValid()) {
    return false;
  }
  return streamRef->unsetOpts(opts);
}

} // namespace vm::internal
