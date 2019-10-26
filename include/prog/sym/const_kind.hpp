#pragma once
#include <iostream>

namespace prog::sym {

enum class ConstKind { Input, Local };

auto operator<<(std::ostream& out, const ConstKind& rhs) -> std::ostream&;

} // namespace prog::sym
