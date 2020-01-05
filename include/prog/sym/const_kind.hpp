#pragma once
#include <iostream>

namespace prog::sym {

enum class ConstKind { Bound, Input, Local };

auto operator<<(std::ostream& out, const ConstKind& rhs) -> std::ostream&;

} // namespace prog::sym
