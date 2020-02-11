#pragma once
#include <cstdint>

namespace vm::internal {

enum class RefFlags : uint8_t {
  None     = 0U,
  GcMarked = 1U,
};

constexpr auto operator|(RefFlags lhs, RefFlags rhs) noexcept {
  return static_cast<RefFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

constexpr auto operator&(RefFlags lhs, RefFlags rhs) noexcept {
  return static_cast<RefFlags>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

constexpr auto operator~(RefFlags rhs) noexcept {
  return static_cast<RefFlags>(~static_cast<uint8_t>(rhs));
}

} // namespace vm::internal
