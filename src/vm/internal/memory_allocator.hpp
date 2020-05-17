#pragma once
#include <cstdint>
#include <utility>

namespace vm::internal {

// Responsible for allocating and deallocating raw memory from the system.
class MemoryAllocator final {
public:
  MemoryAllocator() noexcept;
  MemoryAllocator(const MemoryAllocator& rhs) = delete;
  MemoryAllocator(MemoryAllocator&& rhs)      = delete;
  ~MemoryAllocator() noexcept                 = default;

  auto operator=(const MemoryAllocator& rhs) -> MemoryAllocator& = delete;
  auto operator=(MemoryAllocator&& rhs) -> MemoryAllocator& = delete;

  [[nodiscard]] auto alloc(unsigned int size) noexcept -> std::pair<void*, uint8_t>;

  auto free(void* memoryPtr, uint8_t tag) noexcept -> void;
};

} // namespace vm::internal
