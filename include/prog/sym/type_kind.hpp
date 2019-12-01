#pragma once
#include <ostream>

namespace prog::sym {

enum class TypeKind {
  Int,
  Bool,
  String,
  UserStruct,
};

auto operator<<(std::ostream& out, const TypeKind& rhs) -> std::ostream&;

} // namespace prog::sym
