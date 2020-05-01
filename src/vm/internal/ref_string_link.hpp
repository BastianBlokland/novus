#pragma once
#include "internal/ref.hpp"
#include "internal/ref_string.hpp"
#include "internal/value.hpp"

namespace vm::internal {

class Allocator;

// A 'StringLink' can be used to create a linked list of StringRef's. Used as an optimization when
// concatenation strings, only when the 'result' is needed is the actual concatenation performed.
// Note: Only forward links are supported as the common case is building up a string forwards, but
// if building a string backwards turns out to be common also we could support a configurable
// direction.
class StringLinkRef final : public Ref {
  friend class Allocator;

public:
  StringLinkRef(const StringLinkRef& rhs) = delete;
  StringLinkRef(StringLinkRef&& rhs)      = delete;
  ~StringLinkRef() noexcept override      = default;

  auto operator=(const StringLinkRef& rhs) -> StringLinkRef& = delete;
  auto operator=(StringLinkRef&& rhs) -> StringLinkRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::StringLink; }

  // Prev can either be a StringRef or another StringLinkRef.
  [[nodiscard]] inline auto getPrev() const noexcept { return m_prev; }

  // The 'value' of the link is either a StringRef or a single character as an int.
  [[nodiscard]] inline auto getVal() const noexcept { return m_val; }

  // Size of the 'value' of the link, either the size of the StringRef or 1 incase of a character.
  [[nodiscard]] inline auto getValSize() const noexcept {
    if (m_val.isRef()) {
      return m_val.getDowncastRef<StringRef>()->getSize();
    }
    // If its not a string it has to be a single character.
    return 1U;
  }

  // Has a collapsed version been computed.
  [[nodiscard]] inline auto isCollapsed() const noexcept { return m_collapsed != nullptr; }

  // A collapsed version of the chain if it has been computed, otherwise null.
  [[nodiscard]] inline auto getCollapsed() const noexcept { return m_collapsed; }

  // Set a collapsed version of the chain.
  // Note: We cannot yet clear the 'prev' and 'val' references as multiple threads might be
  // accessing this same link. Instead we wait until the next gc cycle before we clear those.
  inline auto setCollapsed(StringRef* stringRef) noexcept { m_collapsed = stringRef; }

  // Clear the 'prev' and 'val references.
  // Note: Should ONLY be called after a 'collapsed' version has been computed and its guaranteed no
  // other thread is still accessing 'prev' and 'val'.
  [[nodiscard]] inline auto clearLink() noexcept {
    assert(m_collapsed != nullptr);
    m_prev = nullptr;
    m_val  = Value();
  }

private:
  Ref* m_prev;
  Value m_val;
  StringRef* m_collapsed;

  inline explicit StringLinkRef(Ref* prev, Value val) noexcept :
      Ref(getKind()), m_prev{prev}, m_val{val}, m_collapsed{nullptr} {

    assert(m_prev != nullptr);
    assert(m_prev->getKind() == RefKind::String || m_prev->getKind() == RefKind::StringLink);
  }
};

inline auto getStringLinkRef(const Value& val) noexcept {
  return val.getDowncastRef<StringLinkRef>();
}

inline auto getStringOrLinkRef(const Value& val) noexcept {
  auto* ref = val.getRef();
  assert(ref->getKind() == RefKind::String || ref->getKind() == RefKind::StringLink);
  return ref;
}

} // namespace vm::internal
