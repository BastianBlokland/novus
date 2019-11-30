#pragma once
#include "internal/value.hpp"

namespace vm::internal {

class Value;

class StructRef final : public Ref {
  friend class Allocator;

public:
  StructRef(const StructRef& rhs)     = delete;
  StructRef(StructRef&& rhs) noexcept = delete;
  ~StructRef()                        = default;

  auto operator=(const StructRef& rhs) -> StructRef& = delete;
  auto operator=(StructRef&& rhs) noexcept -> StructRef& = delete;

  [[nodiscard]] inline auto getFieldCount() const noexcept { return m_fieldCount; }

  [[nodiscard]] inline auto getField(uint8_t index) const noexcept {
    assert(index < m_fieldCount);
    return *(m_fields + index);
  }

private:
  const Value* m_fields;
  uint8_t m_fieldCount;

  inline explicit StructRef(const Value* fields, uint8_t fieldCount) :
      Ref(RefKind::Struct), m_fields{fields}, m_fieldCount{fieldCount} {}
};

inline auto getStructRef(const Value& val) noexcept {
  auto* ref = val.getRef();
  assert(ref->getKind() == RefKind::Struct);
  return static_cast<StructRef*>(ref); // NOLINT: Down-cast.
}

} // namespace vm::internal
