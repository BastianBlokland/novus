#pragma once
#include "internal/ref.hpp"
#include "internal/value.hpp"
#include <cassert>

namespace vm::internal {

class StringRef final : public Ref {
  friend class RefAllocator;

public:
  StringRef(const StringRef& rhs) = delete;
  StringRef(StringRef&& rhs)      = delete;
  ~StringRef() noexcept           = default;

  auto operator=(const StringRef& rhs) -> StringRef& = delete;
  auto operator=(StringRef&& rhs) -> StringRef& = delete;

  [[nodiscard]] constexpr static auto getKind() { return RefKind::String; }

  [[nodiscard]] inline auto getDataPtr() const noexcept { return m_data; }

  [[nodiscard]] inline auto getCharDataPtr() const noexcept {
    return reinterpret_cast<char*>(m_data);
  }
  [[nodiscard]] inline auto getCharDataPtrEnd() const noexcept {
    return reinterpret_cast<char*>(m_data + m_size);
  }

  [[nodiscard]] inline auto getSize() const noexcept { return m_size; }

  // Note: Size can only be updated to be less then the original.
  inline auto updateSize(unsigned int size) noexcept {
    assert(size <= m_size);
    m_size = size;

    // Null-terminate.
    m_data[size] = '\0';
  }

private:
  unsigned int m_size;
  uint8_t* m_data;

  inline explicit StringRef(uint8_t* data, unsigned int size) noexcept :
      Ref(getKind()), m_size{size}, m_data{data} {}
};

} // namespace vm::internal
