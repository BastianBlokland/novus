#pragma once
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"

namespace vm::internal {

[[nodiscard]] auto getWorkingDirPath(RefAllocator* refAlloc) noexcept -> StringRef*;

} // namespace vm::internal
