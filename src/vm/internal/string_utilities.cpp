#include "internal/string_utilities.hpp"
#include <charconv>
#include <cstdio>

namespace vm::internal {

auto toString(Allocator* allocator, int32_t val) -> Value {
  static const auto maxCharSize = 11;

  const auto strRefAlloc = allocator->allocStr(maxCharSize);
  const auto convRes     = std::to_chars(strRefAlloc.second, strRefAlloc.second + maxCharSize, val);
  if (convRes.ec != std::errc()) {
    throw std::logic_error{"Failed to convert integer to string"};
  }
  strRefAlloc.first->updateSize(convRes.ptr - strRefAlloc.second);
  return internal::refValue(strRefAlloc.first);
}

auto toString(Allocator* allocator, float val) -> Value {
  // NOLINTNEXTLINE: C-style var-arg func, needed because clang is missing std::to_chars(float).
  const auto charSize    = std::snprintf(nullptr, 0, "%.6g", val) + 1; // +1: null-terminator.
  const auto strRefAlloc = allocator->allocStr(charSize);

  // NOLINTNEXTLINE: C-style var-arg func, needed because clang is missing std::to_chars(float).
  std::snprintf(strRefAlloc.second, charSize, "%.6g", val);

  // Remove the null-terminator from the size. Our strings don't use a null-terminator but
  // snprintf always outputs one.
  strRefAlloc.first->updateSize(charSize - 1);

  return internal::refValue(strRefAlloc.first);
}

auto toString(Allocator* allocator, uint8_t val) -> Value {
  const auto strRefAlloc = allocator->allocStr(1);
  *strRefAlloc.second    = val;
  return internal::refValue(strRefAlloc.first);
}

auto getStringLength(Value val) -> int32_t {
  auto* str = getStringRef(val);
  return str->getSize();
}

auto checkStringEq(Value a, Value b) -> bool {
  auto* strA = getStringRef(a);
  auto* strB = getStringRef(b);

  return (strA->getSize() == strB->getSize()) &&
      std::memcmp(strA->getDataPtr(), strB->getDataPtr(), strA->getSize()) == 0;
}

auto concatString(Allocator* allocator, Value a, Value b) -> Value {
  auto* strA = getStringRef(a);
  auto* strB = getStringRef(b);

  // Make a new string big enough to fit both and copy both there.
  auto result = allocator->allocStr(strA->getSize() + strB->getSize());

  std::memcpy(result.second, strA->getDataPtr(), strA->getSize());
  std::memcpy(result.second + strA->getSize(), strB->getDataPtr(), strB->getSize());

  return internal::refValue(result.first);
}

} // namespace vm::internal
