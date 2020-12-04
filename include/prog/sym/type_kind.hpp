#pragma once
#include <ostream>

namespace prog::sym {

enum class TypeKind {
  Int,
  Long,
  Float,
  Bool,
  String,
  Char,
  Stream,
  Struct,
  Union,
  Enum,
  Delegate,
  Future,
  Lazy,
  Process,
};

[[nodiscard]] auto isPrimitive(const TypeKind& kind) -> bool;

auto operator<<(std::ostream& out, const TypeKind& rhs) -> std::ostream&;

} // namespace prog::sym
