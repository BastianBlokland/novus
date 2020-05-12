#pragma once
#include "charconv.hpp"
#include "internal/allocator.hpp"
#include "internal/likely.hpp"
#include "internal/ref_string.hpp"
#include "internal/string_link_utilities.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>

namespace vm::internal {

// Get a StringRef* from a value. Supports direct StringRef's or StringLinkRefs.
// Requires a allocator as in-case of a StringLinkRef we might need to allocate a new string.
inline auto getStringRef(Allocator* allocator, const Value& val) noexcept {
  auto* ref = val.getRef();
  if (ref->getKind() == RefKind::String) {
    auto strRef = downcastRef<StringRef>(ref);
    // Assert that the string is null-terminated.
    assert(strRef->getDataPtr()[strRef->getSize()] == '\0');
    return strRef;
  }

  // If its not a string we assume its a string-link.
  assert(ref->getKind() == RefKind::StringLink);

  // Collapse the string-link chain into a normal string.
  auto* strLinkRef = val.getDowncastRef<StringLinkRef>();
  return collapseStringLink(allocator, *strLinkRef);
}

template <typename IntType>
[[nodiscard]] auto inline intToString(Allocator* allocator, IntType val) noexcept -> StringRef* {
  static_assert(std::is_same<IntType, int32_t>::value || std::is_same<IntType, int64_t>::value);
  static const auto maxCharSize = std::is_same<IntType, int32_t>::value ? 11 : 20;

  const auto strRefAlloc = allocator->allocStr(maxCharSize);
  if (unlikely(strRefAlloc.first == nullptr)) {
    return nullptr;
  }
  auto* charDataPtr = reinterpret_cast<char*>(strRefAlloc.second);

#ifdef HAS_CHAR_CONV
  const auto convRes      = std::to_chars(charDataPtr, charDataPtr + maxCharSize, val);
  const auto resultSize   = convRes.ptr - charDataPtr;
  charDataPtr[resultSize] = '\0'; // Null-terminate.
  strRefAlloc.first->updateSize(convRes.ptr - charDataPtr);
#else
  const char* format;
  if constexpr (std::is_same<IntType, int>::value) {
    format = "%d";
  } else if constexpr (std::is_same<IntType, long>::value) {
    format = "%ld";
  } else if constexpr (std::is_same<IntType, long long>::value) {
    format = "%lld";
  } else {
    static_assert(std::is_same<IntType, bool>::value, "Unsupported integer type");
  }

  // NOLINTNEXTLINE: C-style var-arg func.
  const auto size = std::snprintf(charDataPtr, maxCharSize + 1, format, val);
  assert(charDataPtr[size] == '\0');
  strRefAlloc.first->updateSize(size);
#endif

  return strRefAlloc.first;
}

[[nodiscard]] auto inline floatToString(Allocator* allocator, float val) noexcept -> StringRef* {
  // In theory the 'nan' case is already covered by 'snprintf' but it seems some implementations
  // return '-nan' instead causing an inconsistency between platforms.
  if (unlikely(std::isnan(val))) {
    const static std::string nanStr = "nan";
    return allocator->allocStrLit(nanStr);
  }

  // NOLINTNEXTLINE: C-style var-arg func.
  const auto charSize    = std::snprintf(nullptr, 0, "%.6g", val) + 1; // +1: null-terminator.
  const auto strRefAlloc = allocator->allocStr(charSize);
  if (unlikely(strRefAlloc.first == nullptr)) {
    return nullptr;
  }
  auto* charDataPtr = reinterpret_cast<char*>(strRefAlloc.second);

#if defined(_WIN32) && (!defined(_MSC_VER) || _MSC_VER < 1400)
  // By default windows will add a leading zero to pad to 3 digits in the exponent, to be consistent
  // with unix we enable the 'two digit' mode.
  _set_output_format(_TWO_DIGIT_EXPONENT);
#endif

  // NOLINTNEXTLINE: C-style var-arg func.
  const auto size = std::snprintf(charDataPtr, charSize, "%.6g", val);
  strRefAlloc.first->updateSize(size);

  return strRefAlloc.first;
}

[[nodiscard]] auto inline charToString(Allocator* allocator, uint8_t val) noexcept -> StringRef* {
  const auto strRefAlloc = allocator->allocStr(1);
  if (unlikely(strRefAlloc.first == nullptr)) {
    return nullptr;
  }

  *strRefAlloc.second = val;
  return strRefAlloc.first;
}

[[nodiscard]] auto inline charsToString(Allocator* allocator, uint8_t a, uint8_t b) noexcept
    -> StringRef* {
  const auto strRefAlloc = allocator->allocStr(2);
  if (unlikely(strRefAlloc.first == nullptr)) {
    return nullptr;
  }

  strRefAlloc.second[0] = a;
  strRefAlloc.second[1] = b;
  return strRefAlloc.first;
}

[[nodiscard]] auto inline toStringRef(Allocator* allocator, const std::string& val) noexcept
    -> StringRef* {
  const auto strRefAlloc = allocator->allocStr(val.length());
  if (unlikely(strRefAlloc.first == nullptr)) {
    return nullptr;
  }

  std::memcpy(strRefAlloc.second, val.data(), val.length() + 1); // +1 to cpy the null-terminator.
  return strRefAlloc.first;
}

[[nodiscard]] auto inline checkStringEq(StringRef* a, StringRef* b) noexcept -> bool {
  return (a->getSize() == b->getSize()) &&
      std::memcmp(a->getDataPtr(), b->getDataPtr(), a->getSize()) == 0;
}

[[nodiscard]] auto inline indexString(StringRef* target, int32_t idx) noexcept -> uint8_t {
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
  const auto alloc     = allocator->allocStr(sliceSize);
  if (alloc.first == nullptr) {
    return nullptr;
  }

  std::memcpy(alloc.second, target->getDataPtr() + start, sliceSize);
  return alloc.first;
}

[[nodiscard]] auto inline concatString(Allocator* allocator, StringRef* a, StringRef* b) noexcept
    -> StringRef* {

  // Make a new string big enough to fit both and copy both there.
  auto alloc = allocator->allocStr(a->getSize() + b->getSize());
  if (unlikely(alloc.first == nullptr)) {
    return nullptr;
  }

  std::memcpy(alloc.second, a->getDataPtr(), a->getSize());
  std::memcpy(alloc.second + a->getSize(), b->getDataPtr(), b->getSize());

  return alloc.first;
}

[[nodiscard]] auto inline appendChar(Allocator* allocator, StringRef* a, uint8_t b) noexcept
    -> StringRef* {

  // Make a new string 1 character bigger and copy the original string + the extra character.
  auto alloc = allocator->allocStr(a->getSize() + 1);
  if (unlikely(alloc.first == nullptr)) {
    return nullptr;
  }

  std::memcpy(alloc.second, a->getDataPtr(), a->getSize());
  alloc.second[a->getSize()] = b;

  return alloc.first;
}

} // namespace vm::internal
