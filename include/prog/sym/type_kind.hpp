#pragma once
#include <ostream>

namespace prog::sym {

// NOTE: The values of this enum are exposed to user code through the reflection api, so keep this
// in mind when thinking about changing them.
enum class TypeKind {
  Int          = 0,
  Long         = 1,
  Float        = 2,
  Bool         = 3,
  Char         = 4,
  String       = 5,
  SysStream    = 6,
  SysProcess   = 7,
  SysIOWatcher = 8,
  Struct       = 9,
  Union        = 10,
  Enum         = 11,
  Delegate     = 12,
  Future       = 13,
  Lazy         = 14,
  StaticInt    = 15,
};

[[nodiscard]] auto isPrimitive(const TypeKind& kind) -> bool;

auto operator<<(std::ostream& out, const TypeKind& rhs) -> std::ostream&;

} // namespace prog::sym
