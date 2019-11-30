#pragma once
#include "internal/ref.hpp"
#include "internal/value.hpp"
#include <string>

namespace vm::internal {

class StringRef final : public Ref {
  friend class Allocator;

public:
  StringRef(const StringRef& rhs)     = delete;
  StringRef(StringRef&& rhs) noexcept = delete;
  ~StringRef()                        = default;

  auto operator=(const StringRef& rhs) -> StringRef& = delete;
  auto operator=(StringRef&& rhs) noexcept -> StringRef& = delete;

  [[nodiscard]] inline auto getDataPtr() const noexcept { return m_data; }
  [[nodiscard]] inline auto getSize() const noexcept { return m_size; }

  inline auto updateSize(unsigned int size) noexcept { m_size = size; }

private:
  const char* m_data;
  unsigned int m_size;

  inline explicit StringRef(const char* data, unsigned int size) :
      Ref(RefKind::String), m_data{data}, m_size{size} {}
};

inline auto getStringRef(const Value& val) noexcept {
  auto* ref = val.getRef();
  assert(ref->getKind() == RefKind::String);
  return static_cast<StringRef*>(ref); // NOLINT: Down-cast.
}

} // namespace vm::internal
