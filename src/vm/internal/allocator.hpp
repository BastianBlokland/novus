#pragma once
#include "internal/ref_string.hpp"
#include "internal/ref_struct.hpp"
#include <utility>

namespace vm::internal {

class Allocator final {
public:
  Allocator();
  Allocator(const Allocator& rhs)     = delete;
  Allocator(Allocator&& rhs) noexcept = delete;
  ~Allocator();

  auto operator=(const Allocator& rhs) -> Allocator& = delete;
  auto operator=(Allocator&& rhs) noexcept -> Allocator& = delete;

  [[nodiscard]] auto allocStrLit(const std::string& literal) -> StringRef*;
  [[nodiscard]] auto allocStr(unsigned int size) -> std::pair<StringRef*, char*>;

  [[nodiscard]] auto allocStruct(uint8_t fieldCount) -> std::pair<StructRef*, Value*>;

private:
  Ref* m_firstRef;
  Ref* m_lastRef;

  auto initRef(Ref* ref) -> void;

  template <typename ConcreteRef>
  auto alloc(const unsigned int payloadsize) -> std::pair<void*, void*> {
    // Make a single allocation of the header and the payload.
    const auto refSize = sizeof(ConcreteRef);
    void* refPtr       = std::malloc(refSize + payloadsize); // NOLINT: Manual memory management.
    void* payloadPtr   = static_cast<char*>(refPtr) + refSize;
    return {refPtr, payloadPtr};
  }
};

} // namespace vm::internal
