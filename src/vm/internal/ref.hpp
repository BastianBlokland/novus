#pragma once
#include "internal/ref_flags.hpp"
#include "internal/ref_kind.hpp"
#include <cassert>

namespace vm::internal {

// Base class for a reference.
class Ref {
  friend class RefAllocator;

public:
  Ref(const Ref& rhs)     = delete;
  Ref(Ref&& rhs) noexcept = delete;
  ~Ref() noexcept         = default;

  auto operator=(const Ref& rhs) -> Ref& = delete;
  auto operator=(Ref&& rhs) -> Ref& = delete;

  auto destroy() noexcept -> void;

  [[nodiscard]] inline auto getKind() const noexcept { return m_kind; }

  template <RefFlags F>
  [[nodiscard]] inline auto hasFlag() const noexcept -> bool {
    return (m_flags & F) == F;
  }

  template <RefFlags F>
  inline auto setFlag() noexcept -> void {
    m_flags = m_flags | F;
  }

  template <RefFlags F>
  inline auto unsetFlag() noexcept -> void {
    m_flags = m_flags & ~F;
  }

protected:
  inline explicit Ref(RefKind kind) noexcept : m_next{nullptr}, m_kind{kind}, m_flags{} {}

  // Get a raw pointer to the begining of the Ref struct. Can be used by ref implementations to
  // calculate their end-pointer.
  // For obvious reasons this is a dangernous api and care must be taken.
  [[nodiscard]] inline auto getPtr() noexcept -> uint8_t* {
    return static_cast<uint8_t*>(static_cast<void*>(&m_next));
  }

private:
  Ref* m_next; // Used by the RefAllocator to track all references.
  RefKind m_kind;
  RefFlags m_flags;
};

// Downcast a reference to a child-type, be sure that the types match before calling this.
template <typename RefType>
inline auto downcastRef(Ref* ref) noexcept -> RefType* {
  assert(ref->getKind() == RefType::getKind());
  return static_cast<RefType*>(ref); // NOLINT: Down-cast.
}

} // namespace vm::internal
