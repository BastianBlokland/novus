#pragma once
#include "charconv.hpp"
#include "internal/intrinsics.hpp"
#include "internal/ref_allocator.hpp"
#include "internal/ref_string.hpp"
#include "internal/string_link_utilities.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>

namespace vm::internal {

inline auto isStringEmpty(const Value& val) noexcept {
  auto* ref = getStringOrLinkRef(val);
  if (ref->getKind() == RefKind::String) {
    return downcastRef<StringRef>(ref)->getSize() == 0;
  }

  // String-links are never empty.
  return false;
}

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

enum FloatToStringFlags : uint8_t {
  FloatToStringNormal           = 0,
  FloatToStringNeverScientific  = 1 << 0,
  FloatToStringAlwaysScientific = 1 << 1,
};

[[nodiscard]] auto inline floatToString(
    RefAllocator* refAlloc, float val, uint8_t precision, FloatToStringFlags flags) noexcept
    -> StringRef* {

  // In theory the 'nan' case is already covered by 'snprintf' but it seems some implementations
  // return '-nan' instead causing an inconsistency between platforms.
  if (unlikely(std::isnan(val))) {
    return refAlloc->allocStrLit("nan", 3);
  }

  const char* format;
  if (flags & FloatToStringNeverScientific) {
    format = "%.*f";
  } else if (flags & FloatToStringAlwaysScientific) {
    format = "%.*e";
  } else {
    format = "%.*g";
  }

  // NOLINTNEXTLINE: C-style var-arg func.
  const auto charSize =
      std::snprintf(nullptr, 0, format, precision, val) + 1; // +1: null-terminator.
  const auto str = refAlloc->allocStr(charSize);
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
  const auto size = std::snprintf(charDataPtr, charSize, format, precision, val);
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

[[nodiscard]] auto inline toStringRef(
    RefAllocator* refAlloc, const char* data, size_t length) noexcept -> StringRef* {
  const auto str = refAlloc->allocStr(length);
  if (unlikely(str == nullptr)) {
    return nullptr;
  }

  std::memcpy(str->getDataPtr(), data, length);
  return str;
}

[[nodiscard]] auto inline toStringRef(RefAllocator* refAlloc, const char* cstr) noexcept
    -> StringRef* {
  return toStringRef(refAlloc, cstr, std::strlen(cstr));
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

} // namespace vm::internal
