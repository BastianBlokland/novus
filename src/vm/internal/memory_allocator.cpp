#include "internal/memory_allocator.hpp"

namespace vm::internal {

auto MemoryAllocator::alloc(unsigned int size) noexcept -> void* { return std::malloc(size); }

auto MemoryAllocator::free(void* memoryPtr) noexcept -> void { std::free(memoryPtr); }

} // namespace vm::internal
