#pragma once
#include "internal/likely.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "internal/ref_string_link.hpp"
#include <cstring>

namespace vm::internal {

// Compute the total size of a string-link.
inline auto getStringLinkSize(StringLinkRef& l) noexcept -> unsigned int {
  // If we've precomputed this chain then we can return that size.
  if (l.isCollapsed()) {
    return l.getCollapsed()->getSize();
  }

  // Otherwise walk the chain to compute the full size.
  auto result = l.getValSize();
  auto* cur   = l.getPrev();
  while (true) {
    if (cur->getKind() == RefKind::String) {
      result += downcastRef<StringRef>(cur)->getSize();
      break;
    }
    auto* curLink = downcastRef<StringLinkRef>(cur);
    if (curLink->isCollapsed()) {
      result += curLink->getCollapsed()->getSize();
      break;
    }

    result += curLink->getValSize();
    cur = curLink->getPrev();
    assert(cur != nullptr);
  }

  return result;
}

// Collapse a string-link into a normal string.
inline auto collapseStringLink(RefAllocator* refAlloc, StringLinkRef& l) noexcept -> StringRef* {
  // If we've collapsed this link before return the previous result.
  if (l.isCollapsed()) {
    return l.getCollapsed();
  }

  // Allocate a new string big enough to the hold the entire chain.
  auto size = getStringLinkSize(l);
  auto str  = refAlloc->allocStr(size);
  if (unlikely(str == nullptr)) {
    return nullptr;
  }

  auto* charDataStartPtr = str->getDataPtr();
  auto* charDataPtr      = charDataStartPtr + size;

  // Utility macro for copying to the end to our string.
#define CPY_STR(STR_SRC)                                                                           \
  {                                                                                                \
    charDataPtr -= (STR_SRC)->getSize();                                                           \
    std::memcpy(charDataPtr, (STR_SRC)->getDataPtr(), (STR_SRC)->getSize());                       \
  }

  // Copy our own value into the string.
  if (l.getVal().isRef()) {
    CPY_STR(l.getVal().getDowncastRef<StringRef>());
  } else {
    *--charDataPtr = static_cast<uint8_t>(l.getVal().getInt());
  }

  // Copy the rest of the chain into the string.
  auto* cur = l.getPrev();
  while (true) {
    if (cur->getKind() == RefKind::String) {
      CPY_STR(downcastRef<StringRef>(cur));
      break;
    }
    auto* curLink = downcastRef<StringLinkRef>(cur);
    if (curLink->isCollapsed()) {
      CPY_STR(curLink->getCollapsed());
      break;
    }

    if (curLink->getVal().isRef()) {
      CPY_STR(curLink->getVal().getDowncastRef<StringRef>());
    } else {
      *--charDataPtr = static_cast<uint8_t>(curLink->getVal().getInt());
    }
    cur = curLink->getPrev();
    assert(cur != nullptr);
  }

#undef CPY_LINK

  // Set the new string as the 'collapsed' representation for that link, this caches the value for
  // future requests on the same link.
  l.setCollapsed(str);

  return str;
}

} // namespace vm::internal
