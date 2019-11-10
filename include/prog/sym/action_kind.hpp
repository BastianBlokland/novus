#pragma once
#include <iostream>

namespace prog::sym {

enum class ActionKind { Intrinsic };

auto operator<<(std::ostream& out, const ActionKind& rhs) -> std::ostream&;

} // namespace prog::sym
