#pragma once
#include "internal/ref.hpp"
#include "internal/value.hpp"
#include <cstdint>

namespace vm::internal {

class LongRef final : public Ref {
  friend class RefAllocator;

public:
  LongRef(const LongRef& rhs) = delete;
  LongRef(LongRef&& rhs)      = delete;
  ~LongRef() noexcept         = default;

  auto operator=(const LongRef& rhs) -> LongRef& = delete;
  auto operator=(LongRef&& rhs) -> LongRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::Long; }

  [[nodiscard]] inline auto getVal() const noexcept { return m_val; }

private:
  int64_t m_val;

  inline explicit LongRef(int64_t val) noexcept : Ref(getKind()), m_val{val} {}
};

inline auto getLong(const Value& val) noexcept -> int64_t {
  // Positive long's (most significant bit always 0) are stored in the value directly, while
  // negative longs are stored as references.
  if (val.isRef()) {
    auto* longRef = val.getDowncastRef<LongRef>();
    return longRef->getVal();
  }
  return val.getPosLong();
}

} // namespace vm::internal
