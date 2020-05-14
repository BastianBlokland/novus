#include "internal/allocator.hpp"
#include "internal/ref_future.hpp"
#include "internal/ref_long.hpp"
#include "internal/ref_string.hpp"
#include "internal/ref_string_link.hpp"
#include "internal/ref_struct.hpp"
#include <atomic>
#include <cstdlib>
#include <new>

namespace vm::internal {

Allocator::Allocator(ExecutorRegistry* execRegistry) noexcept :
    m_gc{this, execRegistry}, m_head{nullptr}, m_bytesUntilNextCollection{gcByteInterval} {}

Allocator::~Allocator() noexcept {
  // Shutdown the gc (this makes sure that any ongoing collections are finished).
  m_gc.terminateCollector();

  /* Delete all allocations. Note this assumes no new allocations are being made while we are
  running the destructor. */

  auto* ref = m_head.load(std::memory_order_acquire);
  while (ref) {
    auto next = ref->m_next;
    Allocator::freeUnsafe(ref);
    ref = next;
  }
}

auto Allocator::allocStr(const unsigned int size) noexcept -> std::pair<StringRef*, uint8_t*> {
  auto mem = alloc<StringRef>(size + 1); // +1 for null-terminator.
  if (unlikely(mem.first == nullptr)) {
    return {nullptr, nullptr};
  }

  auto payloadPtr  = static_cast<uint8_t*>(mem.second);
  payloadPtr[size] = '\0'; // Null-terminate the payload.
  auto* refPtr     = static_cast<StringRef*>(new (mem.first) StringRef{payloadPtr, size});
  initRef(refPtr);
  return {refPtr, payloadPtr};
}

auto Allocator::allocStrLit(const std::string& lit) noexcept -> StringRef* {
  auto mem = alloc<StringRef>(0);
  if (unlikely(mem.first == nullptr)) {
    return nullptr;
  }

  auto litSize   = static_cast<unsigned int>(lit.size());
  auto* charData = reinterpret_cast<const uint8_t*>(lit.data());
  auto* refPtr   = static_cast<StringRef*>(new (mem.first) StringRef{charData, litSize});
  initRef(refPtr);
  return refPtr;
}

auto Allocator::allocStrLink(Ref* prev, Value val) noexcept -> StringLinkRef* {
  auto mem = alloc<StringLinkRef>(0);
  if (unlikely(mem.first == nullptr)) {
    return nullptr;
  }

  auto* refPtr = static_cast<StringLinkRef*>(new (mem.first) StringLinkRef{prev, val});
  initRef(refPtr);
  return refPtr;
}

auto Allocator::allocStruct(uint8_t fieldCount) noexcept -> std::pair<StructRef*, Value*> {
  auto mem = alloc<StructRef>(sizeof(Value) * fieldCount);
  if (unlikely(mem.first == nullptr)) {
    return {nullptr, nullptr};
  }

  auto fieldsPtr = static_cast<Value*>(mem.second);
  auto* refPtr   = static_cast<StructRef*>(new (mem.first) StructRef{fieldsPtr, fieldCount});
  initRef(refPtr);
  return {refPtr, fieldsPtr};
}

auto Allocator::initRef(Ref* ref) noexcept -> void {
  // Keep track of all allocated references by linking them as a singly linked list.
  ref->m_next = m_head.load(std::memory_order_relaxed);
  while (!m_head.compare_exchange_weak(
      ref->m_next, ref, std::memory_order_release, std::memory_order_relaxed)) {
  }
}

} // namespace vm::internal
