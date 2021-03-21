#pragma once
#include <ostream>

namespace prog::sym {

// NOTE: The values of this enum are exposed to user code through the reflection api, so keep this
// in mind when thinking about changing them.
enum class TypeKind {
  Int        = 0,
  Long       = 1,
  Float      = 2,
  Bool       = 3,
  Char       = 4,
  String     = 5,
  SysStream  = 6,
  SysProcess = 7,
  Struct     = 8,
  Union      = 9,
  Enum       = 10,
  Delegate   = 11,
  Future     = 12,
  Lazy       = 13,
  StaticInt  = 14,
};

[[nodiscard]] auto isPrimitive(const TypeKind& kind) -> bool;

auto operator<<(std::ostream& out, const TypeKind& rhs) -> std::ostream&;

} // namespace prog::sym
