#pragma once
#include "internal/ref.hpp"
#include "internal/value.hpp"
#include <atomic>
#include <cstdint>

namespace vm::internal {

class AtomicRef final : public Ref {
  friend class RefAllocator;

public:
  AtomicRef(const AtomicRef& rhs) = delete;
  AtomicRef(AtomicRef&& rhs)      = delete;
  ~AtomicRef() noexcept           = default;

  auto operator=(const AtomicRef& rhs) -> AtomicRef& = delete;
  auto operator=(AtomicRef&& rhs) -> AtomicRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::Atomic; }

  [[nodiscard]] inline auto compareAndSwap(int32_t expected, int32_t desired) noexcept -> int32_t {
    m_atomic.compare_exchange_strong(expected, desired, std::memory_order_seq_cst);
    return expected;
  }

  [[nodiscard]] inline auto load() const -> int32_t {
    return m_atomic.load(std::memory_order_seq_cst);
  }

private:
  std::atomic<int32_t> m_atomic;

  inline explicit AtomicRef(int32_t val) noexcept : Ref(getKind()), m_atomic{val} {}
};

inline auto getAtomic(const Value& val) noexcept { return val.getDowncastRef<AtomicRef>(); }

} // namespace vm::internal
