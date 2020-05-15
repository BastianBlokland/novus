#pragma once
#include "internal/ref.hpp"
#include "internal/value.hpp"

namespace vm::internal {

class StringRef final : public Ref {
  friend class Allocator;

public:
  StringRef(const StringRef& rhs) = delete;
  StringRef(StringRef&& rhs)      = delete;
  ~StringRef() noexcept           = default;

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
  unsigned int m_size;
  const uint8_t* m_data;

  inline explicit StringRef(const uint8_t* data, unsigned int size) noexcept :
      Ref(getKind()), m_size{size}, m_data{data} {}
};

} // namespace vm::internal
