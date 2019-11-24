#pragma once
#include "internal/ref_kind.hpp"

namespace vm::internal {

class Ref {
  friend class Allocator;

public:
  Ref(const Ref& rhs)     = delete;
  Ref(Ref&& rhs) noexcept = delete;
  ~Ref()                  = default;

  auto operator=(const Ref& rhs) -> Ref& = delete;
  auto operator=(Ref&& rhs) noexcept -> Ref& = delete;

  [[nodiscard]] inline auto getKind() const noexcept { return m_kind; }

protected:
  inline explicit Ref(RefKind kind) : m_next{nullptr}, m_kind{kind} {}

private:
  Ref* m_next;
  RefKind m_kind;
};

} // namespace vm::internal
