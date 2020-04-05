#pragma once
#include "internal/ref.hpp"
#include "internal/value.hpp"
#include <string>

namespace vm::internal {

class StringRef final : public Ref {
  friend class Allocator;

public:
  StringRef(const StringRef& rhs) = delete;
  StringRef(StringRef&& rhs)      = delete;
  ~StringRef() noexcept override  = default;

  auto operator=(const StringRef& rhs) -> StringRef& = delete;
  auto operator=(StringRef&& rhs) -> StringRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::String; }

  [[nodiscard]] inline auto getDataPtr() const noexcept { return m_data; }

  [[nodiscard]] inline auto getCharDataPtr() const noexcept {
    return reinterpret_cast<const char*>(m_data);
  }

  [[nodiscard]] inline auto getSize() const noexcept { return m_size; }

  inline auto updateSize(unsigned int size) noexcept { m_size = size; }

private:
  const uint8_t* m_data;
  unsigned int m_size;

  inline explicit StringRef(const uint8_t* data, unsigned int size) noexcept :
      Ref(getKind()), m_data{data}, m_size{size} {}
};

inline auto getStringRef(const Value& val) noexcept {
  auto* strRef = val.getDowncastRef<StringRef>();
  // Assert that the string is null-terminated.
  assert(strRef->getDataPtr()[strRef->getSize()] == '\0');
  return strRef;
}

} // namespace vm::internal
