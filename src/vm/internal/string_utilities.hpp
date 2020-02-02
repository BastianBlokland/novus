#pragma once
#include "internal/allocator.hpp"

namespace vm::internal {

[[nodiscard]] auto emptyString(Allocator* allocator) noexcept -> Value;

[[nodiscard]] auto toString(Allocator* allocator, int32_t val) noexcept -> Value;

[[nodiscard]] auto toString(Allocator* allocator, float val) noexcept -> Value;

[[nodiscard]] auto toString(Allocator* allocator, uint8_t val) noexcept -> Value;

[[nodiscard]] auto toString(Allocator* allocator, const std::string& val) noexcept -> Value;

[[nodiscard]] auto toString(Allocator* allocator, const char* val) noexcept -> Value;

[[nodiscard]] auto getStringLength(Value val) noexcept -> int32_t;

[[nodiscard]] auto checkStringEq(Value a, Value b) noexcept -> bool;

[[nodiscard]] auto indexString(Value target, int32_t idx) noexcept -> char;

[[nodiscard]] auto
sliceString(Allocator* allocator, Value target, int32_t start, int32_t end) noexcept -> Value;

[[nodiscard]] auto concatString(Allocator* allocator, Value a, Value b) noexcept -> Value;

} // namespace vm::internal
