#pragma once
#include <iostream>

namespace prog::sym {

enum class ConstKind { Input = 0, Bound = 1, Local = 2 };

auto operator<<(std::ostream& out, const ConstKind& rhs) -> std::ostream&;

} // namespace prog::sym
