#pragma once
#include "internal/value.hpp"

namespace vm::internal {

class Value;

// Structure containing multiple values.
// Note: the fields of the struct are allocated right after this class.
class StructRef final : public Ref {
  friend class RefAllocator;

public:
  StructRef(const StructRef& rhs) = delete;
  StructRef(StructRef&& rhs)      = delete;
  ~StructRef() noexcept           = default;

  auto operator=(const StructRef& rhs) -> StructRef& = delete;
  auto operator=(StructRef&& rhs) -> StructRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::Struct; }

  // Get a pointer to the first field (In memory right after this class).
  [[nodiscard]] inline auto getFieldsBegin() noexcept -> Value* {
    return static_cast<Value*>(static_cast<void*>(getPtr() + sizeof(StructRef)));
  }

  [[nodiscard]] inline auto getFieldsEnd() noexcept -> const Value* {
    return getFieldsBegin() + m_fieldCount;
  }

  [[nodiscard]] inline auto getFieldCount() const noexcept { return m_fieldCount; }

  [[nodiscard]] inline auto getField(uint8_t index) noexcept { return *getFieldPtr(index); }

  [[nodiscard]] inline auto getFieldPtr(uint8_t index) noexcept -> Value* {
    assert(index < getFieldCount());
    return getFieldsBegin() + index;
  }

  [[nodiscard]] inline auto getLastField() noexcept {
    return *(getFieldsBegin() + m_fieldCount - 1);
  }

private:
  uint8_t m_fieldCount;

  inline explicit StructRef(uint8_t fieldCount) noexcept :
      Ref(getKind()), m_fieldCount{fieldCount} {}
};

inline auto getStructRef(const Value& val) noexcept { return val.getDowncastRef<StructRef>(); }

} // namespace vm::internal
