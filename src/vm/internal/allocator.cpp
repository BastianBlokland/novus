#include "internal/allocator.hpp"
#include "internal/ref_string.hpp"
#include <cstdlib>
#include <new>

namespace vm::internal {

Allocator::Allocator() noexcept : m_firstRef{nullptr}, m_lastRef{nullptr} {}

Allocator::~Allocator() noexcept {
  // Delete all allocations.
  auto* ref = m_firstRef;
  while (ref) {
    auto next = ref->m_next;
    std::free(ref); // NOLINT: Manual memory management.
    ref = next;
  }
}

auto Allocator::allocStrLit(const std::string& lit) noexcept -> StringRef* {
  auto mem     = alloc<StringRef>(0);
  auto litSize = static_cast<unsigned int>(lit.size());
  auto* refPtr = static_cast<StringRef*>(new (mem.first) StringRef{lit.data(), litSize});
  initRef(refPtr);
  return refPtr;
}

auto Allocator::allocStr(const unsigned int size) noexcept -> std::pair<StringRef*, char*> {
  auto mem         = alloc<StringRef>(size + 1); // +1 for null-terminator.
  auto payloadPtr  = static_cast<char*>(mem.second);
  payloadPtr[size] = '\0'; // Null-terminate the payload.
  auto* refPtr     = static_cast<StringRef*>(new (mem.first) StringRef{payloadPtr, size});
  initRef(refPtr);
  return {refPtr, payloadPtr};
}

auto Allocator::allocStruct(uint8_t fieldCount) noexcept -> std::pair<StructRef*, Value*> {
  auto mem       = alloc<StructRef>(sizeof(Value) * fieldCount);
  auto fieldsPtr = static_cast<Value*>(mem.second);
  auto* refPtr   = static_cast<StructRef*>(new (mem.first) StructRef{fieldsPtr, fieldCount});
  initRef(refPtr);
  return {refPtr, fieldsPtr};
}

auto Allocator::initRef(Ref* ref) noexcept -> void {
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
