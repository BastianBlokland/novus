#pragma once
#include <ostream>

namespace prog::sym {

enum class TypeKind {
  Int,
  Float,
  Bool,
  String,
  UserStruct,
  UserUnion,
  UserDelegate,
};

auto operator<<(std::ostream& out, const TypeKind& rhs) -> std::ostream&;

} // namespace prog::sym
