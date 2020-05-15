#pragma once
#include "internal/ref.hpp"
#include <cassert>
#include <cstdint>

namespace vm::internal {

static const uint64_t refTag  = static_cast<uint64_t>(1U);
static const uint64_t valMask = ~static_cast<uint64_t>(1U);

// Storage for a novus 'value'. Can either store 63 bits of data or a pointer to a reference.
class Value final {
  friend auto uintValue(uint32_t val) noexcept -> Value;
  friend auto intValue(int32_t val) noexcept -> Value;
  friend auto posLongValue(int64_t val) noexcept -> Value;
  friend auto floatValue(float val) noexcept -> Value;
  friend auto refValue(Ref* ref) noexcept -> Value;
  friend auto nullRefValue() noexcept -> Value;
  template <typename Type>
  friend auto rawPtrValue(Type* ptr) noexcept -> Value;

  static_assert(
      sizeof(uintptr_t) <= sizeof(uint64_t)); // Assert we have enough space for a pointer.

  static_assert(
      alignof(Ref) > 1); // Assert that the alignment of 'Ref' leaves us a bit to store a tag.

  static_assert(sizeof(float) == sizeof(uint32_t)); // Assert that floats are 32 bit.

public:
  Value() = default;

  [[nodiscard]] inline auto isRef() const noexcept -> bool { return (m_raw & refTag) != 0U; }

  [[nodiscard]] inline auto getUInt() const noexcept -> uint32_t {
    assert(!isRef());
    return static_cast<uint32_t>(m_raw >> 32U);
  }

  [[nodiscard]] inline auto getInt() const noexcept -> int32_t {
    assert(!isRef());
    auto upperRaw = static_cast<uint32_t>(m_raw >> 32U);
    return reinterpret_cast<int32_t&>(upperRaw); // NOLINT: Reinterpret cast
  }

  [[nodiscard]] inline auto getPosLong() const noexcept -> int64_t {
    assert(!isRef());
    return static_cast<int64_t>(m_raw >> 1U);
  }

  [[nodiscard]] inline auto getFloat() const noexcept -> float {
    assert(!isRef());
    auto upperRaw = static_cast<uint32_t>(m_raw >> 32U);
    return reinterpret_cast<float&>(upperRaw); // NOLINT: Reinterpret cast
  }

  [[nodiscard]] inline auto getRef() const noexcept -> Ref* {
    assert(isRef());
    // Mask of the tag and interpret it as a pointer (works because due to alignment lowest bit is
    // always 0).
    return reinterpret_cast<Ref*>(m_raw & valMask); // NOLINT: Reinterpret cast
  }

  [[nodiscard]] inline auto isNullRef() const noexcept -> bool {
    assert(isRef());
    return m_raw == refTag; // Check if all bits other then the ref tag are zero.
  }

  template <typename RefType>
  [[nodiscard]] inline auto getDowncastRef() const noexcept -> RefType* {
    return downcastRef<RefType>(getRef());
  }

  template <typename Type>
  [[nodiscard]] inline auto getRawPtr() const noexcept -> Type* {
    assert(!isRef());
    return reinterpret_cast<Type*>(m_raw); // NOLINT: Reinterpret cast
  }

private:
  uint64_t m_raw;

  inline explicit Value(uint64_t raw) noexcept : m_raw{raw} {}
};

[[nodiscard]] inline auto uintValue(uint32_t val) noexcept -> Value {
  // UInt's are stored in the upper 32 bit of the raw value.
  return Value{static_cast<uint64_t>(val) << 32U};
}

[[nodiscard]] inline auto intValue(int32_t val) noexcept -> Value {
  // Int's are stored in the upper 32 bit of the raw value.
  auto upperRaw = reinterpret_cast<uint32_t&>(val); // NOLINT: Reinterpret cast
  return Value{static_cast<uint64_t>(upperRaw) << 32U};
}

[[nodiscard]] inline auto posLongValue(int64_t val) noexcept -> Value {
  assert(val >= 0L);

  // Positive longs (most significant bit is always zero), can be stored in the upper 63 bits.
  return Value{static_cast<uint64_t>(val) << 1U};
}

[[nodiscard]] inline auto floatValue(float val) noexcept -> Value {
  // Float's are stored in the upper 32 bit of the raw value.
  auto upperRaw = reinterpret_cast<uint32_t&>(val); // NOLINT: Reinterpret cast
  return Value{static_cast<uint64_t>(upperRaw) << 32U};
}

[[nodiscard]] inline auto refValue(Ref* ref) noexcept -> Value {
  assert(ref != nullptr);

  // First store the pointer in a variable of the native pointer size (might be 32 bit).
  auto rawRef = reinterpret_cast<uintptr_t>(ref); // NOLINT: Reinterpret cast

  // Then expand to 64 bit and tag it.
  return Value{static_cast<uint64_t>(rawRef) | refTag};
}

[[nodiscard]] inline auto nullRefValue() noexcept -> Value { return Value{refTag}; }

template <typename Type>
[[nodiscard]] inline auto rawPtrValue(Type* ptr) noexcept -> Value {
  assert(ptr != nullptr);

  auto ptrVal = reinterpret_cast<uintptr_t>(ptr); // NOLINT: Reinterpret cast

  // Assert that the least significant bit is 0, reason is we use that bit to signify that this
  // value is a vm 'ref'.
  static_assert(alignof(Type) > 1);
  assert((ptrVal & refTag) == 0U);

  return Value(ptrVal);
}

} // namespace vm::internal
