#pragma once
#include <cstdlib>

namespace vm::internal {

// Responsible for allocating and deallocating raw memory from the system.
class MemoryAllocator final {
public:
  MemoryAllocator() noexcept                  = default;
  MemoryAllocator(const MemoryAllocator& rhs) = delete;
  MemoryAllocator(MemoryAllocator&& rhs)      = delete;
  ~MemoryAllocator() noexcept                 = default;

  auto operator=(const MemoryAllocator& rhs) -> MemoryAllocator& = delete;
  auto operator=(MemoryAllocator&& rhs) -> MemoryAllocator& = delete;

  [[nodiscard]] auto alloc(unsigned int size) noexcept -> void*;

  auto free(void* memoryPtr) noexcept -> void;
};

} // namespace vm::internal
