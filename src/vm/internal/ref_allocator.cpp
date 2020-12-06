#include "internal/ref_allocator.hpp"
#include "internal/ref_future.hpp"
#include "internal/ref_long.hpp"
#include "internal/ref_string.hpp"
#include "internal/ref_string_link.hpp"
#include "internal/ref_struct.hpp"
#include <atomic>
#include <new>

namespace vm::internal {

RefAllocator::RefAllocator(MemoryAllocator* memAlloc) noexcept :
    m_memAlloc(memAlloc), m_head{nullptr} {}

RefAllocator::~RefAllocator() noexcept {
  /* Delete all allocations. Note this assumes no new allocations are being made while we are
  running the destructor. */

  auto* ref = m_head.load(std::memory_order_acquire);
  while (ref) {
    auto next = ref->m_next;
    RefAllocator::freeUnsafe(ref);
    ref = next;
  }
}

auto RefAllocator::subscribe(RefAllocObserver* observer) -> void {
  // Only allowed to be called before allocating any references.
  assert(m_head.load(std::memory_order_acquire) == nullptr);

  m_observers.push_back(observer);
}

auto RefAllocator::allocStr(const unsigned int size) noexcept -> StringRef* {
  auto mem = alloc<StringRef>(size + 1); // +1 for null-terminator.
  if (unlikely(mem.refPtr == nullptr)) {
    return nullptr;
  }

  auto payloadPtr  = static_cast<uint8_t*>(mem.payloadPtr);
  payloadPtr[size] = '\0'; // Null-terminate the payload.
  auto* refPtr     = static_cast<StringRef*>(new (mem.refPtr) StringRef{payloadPtr, size});
  initRef(refPtr, mem.memTag);
  return refPtr;
}

auto RefAllocator::allocStrLit(const char* literalCStr) noexcept -> StringRef* {
  return allocStrLit(literalCStr, std::strlen(literalCStr));
}

auto RefAllocator::allocStrLit(const char* literal, size_t literalLength) noexcept -> StringRef* {
  auto mem = alloc<StringRef>(0);
  if (unlikely(mem.refPtr == nullptr)) {
    return nullptr;
  }

  // Note: The resulting string ref has a mutable pointer to the memory held by the literal, for
  // obvious reasons this is dangerous. Its up to the caller to make sure not to change string
  // backed by literals.

  auto litSize   = static_cast<unsigned int>(literalLength);
  auto* charData = const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(literal));
  auto* refPtr   = static_cast<StringRef*>(new (mem.refPtr) StringRef{charData, litSize});
  initRef(refPtr, mem.memTag);
  return refPtr;
}

auto RefAllocator::allocStrLink(Ref* prev, Value val) noexcept -> StringLinkRef* {
  auto mem = alloc<StringLinkRef>(0);
  if (unlikely(mem.refPtr == nullptr)) {
    return nullptr;
  }

  auto* refPtr = static_cast<StringLinkRef*>(new (mem.refPtr) StringLinkRef{prev, val});
  initRef(refPtr, mem.memTag);
  return refPtr;
}

auto RefAllocator::allocStruct(uint8_t fieldCount) noexcept -> StructRef* {
  auto mem = alloc<StructRef>(sizeof(Value) * fieldCount);
  if (unlikely(mem.refPtr == nullptr)) {
    return nullptr;
  }

  auto* refPtr = static_cast<StructRef*>(new (mem.refPtr) StructRef{fieldCount});
  initRef(refPtr, mem.memTag);
  return refPtr;
}

auto RefAllocator::initRef(Ref* ref, uint8_t memTag) noexcept -> void {

  // Store the memory-tag as we need it when free-ing the memory.
  ref->m_memTag = memTag;

  // Keep track of all allocated references by linking them as a singly linked list.
  ref->m_next = m_head.load(std::memory_order_relaxed);
  while (!m_head.compare_exchange_weak(
      ref->m_next, ref, std::memory_order_release, std::memory_order_relaxed)) {
  }
}

} // namespace vm::internal
