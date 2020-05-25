#pragma once
#include "charconv.hpp"
#include "internal/likely.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "internal/string_link_utilities.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>

namespace vm::internal {

// Get a StringRef* from a value. Supports direct StringRef's or StringLinkRefs.
// Requires a allocator as in-case of a StringLinkRef we might need to allocate a new string.
inline auto getStringRef(RefAllocator* refAlloc, const Value& val) noexcept {
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
  return collapseStringLink(refAlloc, *strLinkRef);
}

template <typename IntType>
[[nodiscard]] auto inline intToString(RefAllocator* refAlloc, IntType val) noexcept -> StringRef* {
  static_assert(std::is_same<IntType, int32_t>::value || std::is_same<IntType, int64_t>::value);
  static const auto maxCharSize = std::is_same<IntType, int32_t>::value ? 11 : 20;

  const auto str = refAlloc->allocStr(maxCharSize);
  if (unlikely(str == nullptr)) {
    return nullptr;
  }
  auto* charDataPtr = str->getCharDataPtr();

#ifdef HAS_CHAR_CONV
  const auto convRes    = std::to_chars(charDataPtr, charDataPtr + maxCharSize, val);
  const auto resultSize = convRes.ptr - charDataPtr;
  str->updateSize(resultSize);
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
  str->updateSize(size);
#endif

  return str;
}

[[nodiscard]] auto inline floatToString(RefAllocator* refAlloc, float val) noexcept -> StringRef* {
  // In theory the 'nan' case is already covered by 'snprintf' but it seems some implementations
  // return '-nan' instead causing an inconsistency between platforms.
  if (unlikely(std::isnan(val))) {
    const static std::string nanStr = "nan";
    return refAlloc->allocStrLit(nanStr);
  }

  // NOLINTNEXTLINE: C-style var-arg func.
  const auto charSize = std::snprintf(nullptr, 0, "%.6g", val) + 1; // +1: null-terminator.
  const auto str      = refAlloc->allocStr(charSize);
  if (unlikely(str == nullptr)) {
    return nullptr;
  }
  auto* charDataPtr = str->getCharDataPtr();

#if defined(_WIN32) && (!defined(_MSC_VER) || _MSC_VER < 1400)
  // By default windows will add a leading zero to pad to 3 digits in the exponent, to be consistent
  // with unix we enable the 'two digit' mode.
  _set_output_format(_TWO_DIGIT_EXPONENT);
#endif

  // NOLINTNEXTLINE: C-style var-arg func.
  const auto size = std::snprintf(charDataPtr, charSize, "%.6g", val);
  str->updateSize(size);

  return str;
}

[[nodiscard]] auto inline charToString(RefAllocator* refAlloc, uint8_t val) noexcept -> StringRef* {
  const auto str = refAlloc->allocStr(1);
  if (unlikely(str == nullptr)) {
    return nullptr;
  }

  *str->getDataPtr() = val;
  return str;
}

[[nodiscard]] auto inline charsToString(RefAllocator* refAlloc, uint8_t a, uint8_t b) noexcept
    -> StringRef* {
  const auto str = refAlloc->allocStr(2);
  if (unlikely(str == nullptr)) {
    return nullptr;
  }

  str->getDataPtr()[0] = a;
  str->getDataPtr()[1] = b;
  return str;
}

[[nodiscard]] auto inline toStringRef(RefAllocator* refAlloc, const std::string& val) noexcept
    -> StringRef* {
  const auto str = refAlloc->allocStr(val.length());
  if (unlikely(str == nullptr)) {
    return nullptr;
  }

  std::memcpy(str->getDataPtr(), val.data(), val.length() + 1); // +1 to copy the null-terminator.
  return str;
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
    RefAllocator* refAlloc, StringRef* target, int32_t start, int32_t end) noexcept -> StringRef* {
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
    return refAlloc->allocStr(0);
  }

  // Copy the slice into a new string.
  const auto sliceSize = end - start;
  const auto str       = refAlloc->allocStr(sliceSize);
  if (str == nullptr) {
    return nullptr;
  }

  std::memcpy(str->getDataPtr(), target->getDataPtr() + start, sliceSize);
  return str;
}

[[nodiscard]] auto inline concatString(RefAllocator* refAlloc, StringRef* a, StringRef* b) noexcept
    -> StringRef* {

  // Make a new string big enough to fit both and copy both there.
  auto str = refAlloc->allocStr(a->getSize() + b->getSize());
  if (unlikely(str == nullptr)) {
    return nullptr;
  }

  std::memcpy(str->getDataPtr(), a->getDataPtr(), a->getSize());
  std::memcpy(str->getDataPtr() + a->getSize(), b->getDataPtr(), b->getSize());

  return str;
}

[[nodiscard]] auto inline appendChar(RefAllocator* refAlloc, StringRef* a, uint8_t b) noexcept
    -> StringRef* {

  // Make a new string 1 character bigger and copy the original string + the extra character.
  auto str = refAlloc->allocStr(a->getSize() + 1);
  if (unlikely(str == nullptr)) {
    return nullptr;
  }

  std::memcpy(str->getDataPtr(), a->getDataPtr(), a->getSize());
  str->getDataPtr()[a->getSize()] = b;

  return str;
}

} // namespace vm::internal
