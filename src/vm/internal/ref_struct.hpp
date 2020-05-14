#pragma once
#include "internal/value.hpp"

namespace vm::internal {

class Value;

class StructRef final : public Ref {
  friend class Allocator;

public:
  StructRef(const StructRef& rhs) = delete;
  StructRef(StructRef&& rhs)      = delete;
  ~StructRef() noexcept           = default;

  auto operator=(const StructRef& rhs) -> StructRef& = delete;
  auto operator=(StructRef&& rhs) -> StructRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::Struct; }

  [[nodiscard]] inline auto getFieldsBegin() const noexcept -> const Value* { return m_fields; }

  [[nodiscard]] inline auto getFieldsEnd() const noexcept -> const Value* {
    return m_fields + m_fieldCount;
  }

  [[nodiscard]] inline auto getFieldCount() const noexcept { return m_fieldCount; }

  [[nodiscard]] inline auto getField(uint8_t index) const noexcept { return *getFieldPtr(index); }

  [[nodiscard]] inline auto getFieldPtr(uint8_t index) const noexcept -> Value* {
    assert(index < getFieldCount());
    return m_fields + index;
  }

  [[nodiscard]] inline auto getLastField() const noexcept { return *(m_fields + m_fieldCount - 1); }

private:
  uint8_t m_fieldCount;
  Value* m_fields;

  inline explicit StructRef(Value* fields, uint8_t fieldCount) noexcept :
      Ref(getKind()), m_fieldCount{fieldCount}, m_fields{fields} {}
};

inline auto getStructRef(const Value& val) noexcept { return val.getDowncastRef<StructRef>(); }

} // namespace vm::internal
