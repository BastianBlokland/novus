#include "internal/string_utilities.hpp"
#include <cstdio>
#include <cstring>

#if __has_include(<charconv>)
#include <charconv>
#define HAS_CHAR_CONV
#endif

namespace vm::internal {

auto emptyString(Allocator* allocator) -> Value {
  auto emptyStrAlloc = allocator->allocStr(0);
  return refValue(emptyStrAlloc.first);
}

auto toString(Allocator* allocator, int32_t val) -> Value {
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
  assert(strRefAlloc.first[size] == '\0');
  strRefAlloc.first->updateSize(size);
#endif

  return refValue(strRefAlloc.first);
}

auto toString(Allocator* allocator, float val) -> Value {
  // NOLINTNEXTLINE: C-style var-arg func.
  const auto charSize    = std::snprintf(nullptr, 0, "%.6g", val) + 1; // +1: null-terminator.
  const auto strRefAlloc = allocator->allocStr(charSize);

  // NOLINTNEXTLINE: C-style var-arg func.
  const auto size = std::snprintf(strRefAlloc.second, charSize, "%.6g", val);
  strRefAlloc.first->updateSize(size);

  return refValue(strRefAlloc.first);
}

auto toString(Allocator* allocator, uint8_t val) -> Value {
  const auto strRefAlloc = allocator->allocStr(1);
  *strRefAlloc.second    = val;
  return refValue(strRefAlloc.first);
}

auto toString(Allocator* allocator, const std::string& val) -> Value {
  const auto strRefAlloc = allocator->allocStr(val.length());
  std::memcpy(strRefAlloc.second, val.data(), val.length() + 1); // +1 to cpy the null-terminator.
  return refValue(strRefAlloc.first);
}

auto toString(Allocator* allocator, const char* val) -> Value {
  const auto len         = std::strlen(val);
  const auto strRefAlloc = allocator->allocStr(len);
  std::memcpy(strRefAlloc.second, val, len + 1); // +1 to cpy the null-terminator.
  return refValue(strRefAlloc.first);
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

auto indexString(Value target, int32_t idx) -> char {
  auto* strTgt = getStringRef(target);
  if (idx < 0 || static_cast<unsigned>(idx) >= strTgt->getSize()) {
    return 0;
  }
  return *(strTgt->getDataPtr() + idx);
}

auto sliceString(Allocator* allocator, Value target, int32_t start, int32_t end) -> Value {
  auto* strTgt       = getStringRef(target);
  const auto tgtSize = strTgt->getSize();
  if (tgtSize == 0) {
    return target;
  }
  if (start < 0) {
    start = 0;
  }
  if (end < 0) {
    end = 0;
  }

  if (static_cast<unsigned>(end) >= tgtSize - 1) {
    end = tgtSize - 1;
    // 'Slice' of the entire string.
    if (start == 0) {
      return target;
    }
  }
  if (start > end) {
    start = end;
  }

  // Copy the slice into a new string.
  const auto sliceSize = 1 + end - start;
  const auto result    = allocator->allocStr(sliceSize);
  std::memcpy(result.second, strTgt->getDataPtr() + start, sliceSize);
  return refValue(result.first);
}

auto concatString(Allocator* allocator, Value a, Value b) -> Value {
  auto* strA = getStringRef(a);
  auto* strB = getStringRef(b);

  // Make a new string big enough to fit both and copy both there.
  auto result = allocator->allocStr(strA->getSize() + strB->getSize());

  std::memcpy(result.second, strA->getDataPtr(), strA->getSize());
  std::memcpy(result.second + strA->getSize(), strB->getDataPtr(), strB->getSize());

  return refValue(result.first);
}

} // namespace vm::internal
