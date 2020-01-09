#pragma once
#include "internal/allocator.hpp"

namespace vm::internal {

[[nodiscard]] auto toString(Allocator* allocator, int32_t val) -> Value;

[[nodiscard]] auto toString(Allocator* allocator, float val) -> Value;

[[nodiscard]] auto toString(Allocator* allocator, uint8_t val) -> Value;

[[nodiscard]] auto getStringLength(Value val) -> int32_t;

[[nodiscard]] auto checkStringEq(Value a, Value b) -> bool;

[[nodiscard]] auto indexString(Value target, int32_t idx) -> char;

[[nodiscard]] auto sliceString(Allocator* allocator, Value target, int32_t start, int32_t end)
    -> Value;

[[nodiscard]] auto concatString(Allocator* allocator, Value a, Value b) -> Value;

} // namespace vm::internal
