#pragma once
#include <iostream>

namespace prog::sym {

enum class FuncKind { Intrinsic, User };

auto operator<<(std::ostream& out, const FuncKind& rhs) -> std::ostream&;

} // namespace prog::sym
