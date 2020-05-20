#pragma once
#include <iostream>

namespace prog::sym {

enum class ConstKind {
  Input = 0, // Input into a function.
  Bound = 1, // Bound from the parent scope of a closure.
  Local = 2  // Normal local constant.
};

auto operator<<(std::ostream& out, const ConstKind& rhs) -> std::ostream&;

} // namespace prog::sym
