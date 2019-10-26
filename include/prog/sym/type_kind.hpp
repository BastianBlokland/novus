#pragma once
#include <iostream>

namespace prog::sym {

enum class TypeKind { Intrinsic };

auto operator<<(std::ostream& out, const TypeKind& rhs) -> std::ostream&;

} // namespace prog::sym
