#pragma once
#include "internal/ref_future.hpp"
#include "internal/ref_string.hpp"
#include "internal/ref_struct.hpp"
#include <utility>

namespace vm::internal {

class Allocator final {
public:
  Allocator() noexcept;
  Allocator(const Allocator& rhs) = delete;
  Allocator(Allocator&& rhs)      = delete;
  ~Allocator() noexcept;

  auto operator=(const Allocator& rhs) -> Allocator& = delete;
  auto operator=(Allocator&& rhs) -> Allocator& = delete;

  [[nodiscard]] auto allocStrLit(const std::string& literal) noexcept -> StringRef*;
  [[nodiscard]] auto allocStr(unsigned int size) noexcept -> std::pair<StringRef*, char*>;

  [[nodiscard]] auto allocStruct(uint8_t fieldCount) noexcept -> std::pair<StructRef*, Value*>;

  [[nodiscard]] auto allocFuture() noexcept -> FutureRef*;

private:
  Ref* m_head;

  auto initRef(Ref* ref) noexcept -> void;

  template <typename ConcreteRef>
  auto alloc(const unsigned int payloadsize) noexcept -> std::pair<void*, void*> {
    // Make a single allocation of the header and the payload.
    const auto refSize = sizeof(ConcreteRef);
    void* refPtr       = std::malloc(refSize + payloadsize); // NOLINT: Manual memory management.
    void* payloadPtr   = static_cast<char*>(refPtr) + refSize;
    return {refPtr, payloadPtr};
  }
};

} // namespace vm::internal
