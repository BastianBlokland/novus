#pragma once
#include <cmath>
#include <limits>
#include <type_traits>

// Saturating float to integer conversion.

namespace conv {

template <typename T>
[[nodiscard]] inline auto saturateCast(float val) noexcept -> T {
  static_assert(std::is_integral_v<T>, "Target of a saturating cast has to be an integer");

  constexpr auto minVal = static_cast<float>(std::numeric_limits<T>::min());
  constexpr auto maxVal = static_cast<float>(std::numeric_limits<T>::max());

  if (std::isnan(val)) {
    return T{0};
  }
  if (val <= minVal) {
    return std::numeric_limits<T>::min();
  }
  if (val >= maxVal) {
    return std::numeric_limits<T>::max();
  }
  return static_cast<T>(val);
}

} // namespace conv
