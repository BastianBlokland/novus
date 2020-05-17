#include "internal/memory_allocator.hpp"

namespace vm::internal {

auto MemoryAllocator::alloc(unsigned int size) noexcept -> std::pair<void*, uint8_t> {
  return {std::malloc(size), 0};
}

auto MemoryAllocator::free(void* memoryPtr, uint8_t /*unused*/) noexcept -> void {
  std::free(memoryPtr);
}

} // namespace vm::internal
