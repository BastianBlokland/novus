#include "internal/platform_utilities.hpp"

namespace vm::internal {

auto getEndianness() noexcept -> Endianness {

  union Mix {
    uint32_t i;
    uint8_t c[4]; // NOLINT(modernize-avoid-c-arrays)
  };

  auto e = Mix{0x01'00'00'00};
  return e.c[0] ? Endianness::Big : Endianness::Little;
}

} // namespace vm::internal
