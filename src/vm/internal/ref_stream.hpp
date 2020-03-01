#pragma once
#include "internal/allocator.hpp"
#include "internal/ref.hpp"
#include "internal/ref_string.hpp"
#include "internal/value.hpp"

namespace vm::internal {

class StreamRef : public Ref {
public:
  StreamRef(const StreamRef& rhs) = delete;
  StreamRef(StreamRef&& rhs)      = delete;
  ~StreamRef() noexcept override  = default;

  auto operator=(const StreamRef& rhs) -> StreamRef& = delete;
  auto operator=(StreamRef&& rhs) -> StreamRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::Stream; }

  [[nodiscard]] virtual auto isValid() noexcept -> bool = 0;

  virtual auto read(Allocator* alloc, int32_t max) noexcept -> StringRef* = 0;
  virtual auto write(StringRef* str) noexcept -> bool                     = 0;
  virtual auto flush() noexcept -> void                                   = 0;

protected:
  inline StreamRef() noexcept : Ref(getKind()) {}
};

inline auto streamCheckValid(const Value& stream) noexcept -> bool {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  return streamRef->isValid();
}

inline auto streamRead(Allocator* alloc, const Value& stream, int max) noexcept -> StringRef* {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  if (!streamRef->isValid()) {
    return alloc->allocStr(0).first;
  }
  return streamRef->read(alloc, max);
}

inline auto streamWrite(const Value& stream, StringRef* str) noexcept -> int32_t {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  if (!streamRef->isValid()) {
    return 0;
  }
  return streamRef->write(str);
}

inline auto streamFlush(const Value& stream) noexcept -> void {
  auto* streamRef = stream.getDowncastRef<StreamRef>();
  if (streamRef->isValid()) {
    streamRef->flush();
  }
}

} // namespace vm::internal
