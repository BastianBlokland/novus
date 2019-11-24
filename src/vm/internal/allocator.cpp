#include "internal/allocator.hpp"
#include "internal/ref_string.hpp"
#include <cstdlib>
#include <new>

namespace vm::internal {

Allocator::Allocator() : m_firstRef{nullptr}, m_lastRef{nullptr} {}

Allocator::~Allocator() {
  // Delete all allocations.
  auto* ref = m_firstRef;
  while (ref) {
    auto next = ref->m_next;
    std::free(ref); // NOLINT: Manual memory management.
    ref = next;
  }
}

auto Allocator::allocStrLit(const std::string& lit) -> StringRef* {
  auto mem     = alloc<StringRef>(0);
  auto litSize = static_cast<unsigned int>(lit.size());
  auto* refPtr = static_cast<StringRef*>(new (mem.first) StringRef{lit.data(), litSize});
  initRef(refPtr);
  return refPtr;
}

auto Allocator::allocStr(const unsigned int size) -> std::pair<StringRef*, char*> {
  auto mem         = alloc<StringRef>(size);
  char* payloadPtr = static_cast<char*>(mem.second);
  auto* refPtr     = static_cast<StringRef*>(new (mem.first) StringRef{payloadPtr, size});
  initRef(refPtr);
  return {refPtr, payloadPtr};
}

auto Allocator::initRef(Ref* ref) -> void {
  // Keep track of all allocated references by linking them as a singly linked list.
  if (m_lastRef) {
    m_lastRef->m_next = ref;
    m_lastRef         = ref;
  } else {
    m_firstRef = ref;
    m_lastRef  = ref;
  }
}

} // namespace vm::internal
