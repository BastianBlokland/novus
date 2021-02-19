#pragma once
#include "internal/ref.hpp"
#include "internal/value.hpp"
#include <cstdint>

namespace vm::internal {

// Reference to a 64 bit integer value.
// We cannot store the full 64 bits in a 'Value' because only 63 bits are available for storage.
// As an optimization longs with the most significant bit set to 0 are stored directly in a 'Value'
// others are stored in a ULongRef.
class ULongRef final : public Ref {
  friend class RefAllocator;

public:
  ULongRef(const ULongRef& rhs) = delete;
  ULongRef(ULongRef&& rhs)      = delete;
  ~ULongRef() noexcept          = default;

  auto operator=(const ULongRef& rhs) -> ULongRef& = delete;
  auto operator=(ULongRef&& rhs) -> ULongRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::ULong; }

  [[nodiscard]] inline auto getVal() const noexcept { return m_val; }

private:
  uint64_t m_val;

  inline explicit ULongRef(uint64_t val) noexcept : Ref(getKind()), m_val{val} {}
};

inline auto getULong(const Value& val) noexcept -> uint64_t {
  // Small long's (most significant bit set to 0) are stored in the value directly, while others
  // are stored as references.
  if (val.isRef()) {
    return val.getDowncastRef<ULongRef>()->getVal();
  }
  return val.getSmallULong();
}

inline auto getLong(const Value& val) noexcept -> int64_t {
  const uint64_t raw = getULong(val);
  return reinterpret_cast<const int64_t&>(raw);
}

} // namespace vm::internal
