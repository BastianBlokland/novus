#pragma once
#include "internal/allocator.hpp"
#include <cstdio>
#include <cstring>

#if __has_include(<charconv>)
#include <charconv>
#define HAS_CHAR_CONV
#endif

namespace vm::internal {

[[nodiscard]] auto inline toString(Allocator* allocator, int32_t val) noexcept -> StringRef* {
  static const auto maxCharSize = 11;
  const auto strRefAlloc        = allocator->allocStr(maxCharSize);

#ifdef HAS_CHAR_CONV
  const auto convRes    = std::to_chars(strRefAlloc.second, strRefAlloc.second + maxCharSize, val);
  const auto resultSize = convRes.ptr - strRefAlloc.second;
  strRefAlloc.second[resultSize] = '\0'; // Null-terminate.
  strRefAlloc.first->updateSize(convRes.ptr - strRefAlloc.second);
#else
  // NOLINTNEXTLINE: C-style var-arg func.
  const auto size = std::snprintf(strRefAlloc.second, maxCharSize + 1, "%d", val);
  assert(strRefAlloc.second[size] == '\0');
  strRefAlloc.first->updateSize(size);
#endif

  return strRefAlloc.first;
}

[[nodiscard]] auto inline toString(Allocator* allocator, float val) noexcept -> StringRef* {
  // NOLINTNEXTLINE: C-style var-arg func.
  const auto charSize    = std::snprintf(nullptr, 0, "%.6g", val) + 1; // +1: null-terminator.
  const auto strRefAlloc = allocator->allocStr(charSize);

  // NOLINTNEXTLINE: C-style var-arg func.
  const auto size = std::snprintf(strRefAlloc.second, charSize, "%.6g", val);
  strRefAlloc.first->updateSize(size);

  return strRefAlloc.first;
}

[[nodiscard]] auto inline toString(Allocator* allocator, uint8_t val) noexcept -> StringRef* {
  const auto strRefAlloc = allocator->allocStr(1);
  *strRefAlloc.second    = val;
  return strRefAlloc.first;
}

[[nodiscard]] auto inline toString(Allocator* allocator, const std::string& val) noexcept
    -> StringRef* {
  const auto strRefAlloc = allocator->allocStr(val.length());
  std::memcpy(strRefAlloc.second, val.data(), val.length() + 1); // +1 to cpy the null-terminator.
  return strRefAlloc.first;
}

[[nodiscard]] auto inline toString(Allocator* allocator, const char* val) noexcept -> StringRef* {
  const auto len         = std::strlen(val);
  const auto strRefAlloc = allocator->allocStr(len);
  std::memcpy(strRefAlloc.second, val, len + 1); // +1 to cpy the null-terminator.
  return strRefAlloc.first;
}

[[nodiscard]] auto inline checkStringEq(StringRef* a, StringRef* b) noexcept -> bool {
  return (a->getSize() == b->getSize()) &&
      std::memcmp(a->getDataPtr(), b->getDataPtr(), a->getSize()) == 0;
}

[[nodiscard]] auto inline indexString(StringRef* target, int32_t idx) noexcept -> char {
  if (idx < 0 || static_cast<unsigned>(idx) >= target->getSize()) {
    return 0;
  }
  return *(target->getDataPtr() + idx);
}

[[nodiscard]] auto inline sliceString(
    Allocator* allocator, StringRef* target, int32_t start, int32_t end) noexcept -> StringRef* {
  const auto tgtSize = target->getSize();

  // Check for negative indicies.
  if (start < 0) {
    start = 0;
  }
  if (end < 0) {
    end = 0;
  }

  // Check that end does not go past the target string.
  if (static_cast<unsigned>(end) >= tgtSize) {
    end = tgtSize;
    // 'Slice' of the entire string.
    if (start == 0) {
      return target;
    }
  }

  // Check for inverted indices.
  if (start > end) {
    start = end;
  }

  if (start == end) {
    return allocator->allocStr(0).first;
  }

  // Copy the slice into a new string.
  const auto sliceSize = end - start;
  const auto result    = allocator->allocStr(sliceSize);
  std::memcpy(result.second, target->getDataPtr() + start, sliceSize);
  return result.first;
}

[[nodiscard]] auto inline concatString(Allocator* allocator, StringRef* a, StringRef* b) noexcept
    -> StringRef* {

  // Make a new string big enough to fit both and copy both there.
  auto result = allocator->allocStr(a->getSize() + b->getSize());

  std::memcpy(result.second, a->getDataPtr(), a->getSize());
  std::memcpy(result.second + a->getSize(), b->getDataPtr(), b->getSize());

  return result.first;
}

} // namespace vm::internal
