#pragma once
#include "gsl.hpp"
#include "internal/executor_registry.hpp"
#include "internal/garbage_collector.hpp"
#include "internal/likely.hpp"
#include "internal/memory_allocator.hpp"
#include "internal/ref_alloc_observer.hpp"
#include <atomic>
#include <utility>

namespace vm::internal {

class FutureRef;
class LongRef;
class StreamRef;
class StringRef;
class StringLinkRef;
class StructRef;

// Reference Allocator is responsible for acquiring raw memory from the MemoryAllocator and then
// initialing references in it.
// Also responsible for keeping track of all live references.
class RefAllocator final {
public:
  RefAllocator(MemoryAllocator* memAlloc) noexcept;
  RefAllocator(const RefAllocator& rhs) = delete;
  RefAllocator(RefAllocator&& rhs)      = delete;
  ~RefAllocator() noexcept;

  auto operator=(const RefAllocator& rhs) -> RefAllocator& = delete;
  auto operator=(RefAllocator&& rhs) -> RefAllocator& = delete;

  // Observe allocations being made.
  // Note: NOT synchronized has to be called before the application makes any allocations.
  auto subscribe(RefAllocObserver* observer) -> void;

  // Allocate a string, upon failure returns nullptr.
  [[nodiscard]] auto allocStr(unsigned int size) noexcept -> StringRef*;

  // Allocate a string from a literal, upon failure returns nullptr.
  [[nodiscard]] auto allocStrLit(const std::string& literal) noexcept -> StringRef*;

  // Allocate a string from a literal, upon failure returns nullptr.
  [[nodiscard]] auto allocStrLit(const char* literal) noexcept -> StringRef*;

  // Allocate a string from a literal, upon failure returns nullptr.
  [[nodiscard]] auto allocStrLit(const char* literal, size_t literalLength) noexcept -> StringRef*;

  // Allocate a string-link, upon failure returns nullptr.
  [[nodiscard]] auto allocStrLink(Ref* prev, Value val) noexcept -> StringLinkRef*;

  // Allocate a struct, upon failure returns nullptr.
  [[nodiscard]] auto allocStruct(uint8_t fieldCount) noexcept -> StructRef*;

  // Allocate a plain ref type, upon failure returns nullptr.
  template <typename RefType, class... ArgTypes>
  [[nodiscard]] auto allocPlain(ArgTypes&&... args) noexcept -> RefType* {
    static_assert(std::is_convertible<RefType*, Ref*>());

    auto mem = alloc<RefType>(0);
    if (unlikely(mem.refPtr == nullptr)) {
      return nullptr;
    }

    auto* refPtr = static_cast<RefType*>(new (mem.refPtr) RefType{std::forward<ArgTypes>(args)...});
    initRef(refPtr, mem.memTag);
    return refPtr;
  }

  // The 'head' allocation is the newest created reference. In combination with the 'getNextAlloc'
  // allows walking all live references.
  [[nodiscard]] inline auto getHeadAlloc() noexcept -> Ref* {
    return m_head.load(std::memory_order_acquire);
  }

  // Retreive the 'next' references for a given reference, allows walking all live references.
  [[nodiscard]] inline auto getNextAlloc(Ref* ref) noexcept -> Ref* { return ref->m_next; }

  // Free the reference after the given one.
  // Note: Not thread-safe, should not be called concurrently.
  // Frees the next one instead of the given one because then we can more efficiently keep our
  // linked list of references up to date.
  inline auto freeNext(Ref* ref) noexcept -> Ref* {
    auto* toFree = ref->m_next;
    if (toFree) {
      auto* next  = toFree->m_next;
      ref->m_next = next;
      freeUnsafe(toFree);
      return next;
    }
    return nullptr;
  }

private:
  struct Allocation {
    void* refPtr;
    void* payloadPtr;
    uint8_t memTag;
  };

  MemoryAllocator* m_memAlloc;
  std::atomic<Ref*> m_head;
  std::vector<RefAllocObserver*> m_observers;

  auto initRef(Ref* ref, uint8_t memTag) noexcept -> void;

  // Allocate raw memory for a structure + a payload for that structure. When 'payloadsize' is 0
  // only enough memory to hold the structure is allocated. When 'payloadsize' is 10 then 10
  // additional bytes are allocated after the structure.
  // Returns a pair of pointers, first points to the memory for the structure, the second points to
  // the payload memory.
  // Note: When memory allocation fails returns {nullptr, nullptr},
  template <typename ConcreteRef>
  inline auto alloc(const unsigned int payloadsize) noexcept -> Allocation {
    // Make a single allocation of the header and the payload.
    const auto refSize   = sizeof(ConcreteRef);
    const auto allocSize = refSize + payloadsize;
    auto alloc           = m_memAlloc->alloc(allocSize);
    void* payloadPtr     = static_cast<char*>(alloc.first) + refSize;

    // Notify any observers about this allocation.
    for (auto* observer : m_observers) {
      observer->notifyAlloc(allocSize);
    }

    return Allocation{alloc.first, payloadPtr, alloc.second};
  }

  // Destruct and free the given ref, unsafe because it doesn't update any of the bookkeeping.
  inline auto freeUnsafe(Ref* ref) noexcept -> void {
    // 'Destroy' the reference, which will call the destructor of the implementation.
    // Note: Reason why its not using a virtual destructor is that this way we can avoid the vtable.
    ref->destroy();
    // Free the backing memory.
    m_memAlloc->free(ref, ref->m_memTag);
  }
};

} // namespace vm::internal
