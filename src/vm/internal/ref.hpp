#pragma once
#include "internal/ref_flags.hpp"
#include "internal/ref_kind.hpp"
#include <cassert>

namespace vm::internal {

class Ref {
  friend class Allocator;

public:
  Ref(const Ref& rhs)     = delete;
  Ref(Ref&& rhs) noexcept = delete;
  virtual ~Ref() noexcept = default;

  auto operator=(const Ref& rhs) -> Ref& = delete;
  auto operator=(Ref&& rhs) -> Ref& = delete;

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

private:
  Ref* m_next;
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
