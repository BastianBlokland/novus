#pragma once
#include <cstdint>

namespace vm::internal {

enum class Endianness : uint8_t {
  Little = 0,
  Big    = 1,
};

[[nodiscard]] auto getEndianness() noexcept -> Endianness;

} // namespace vm::internal
