#include "prog/sym/type_kind.hpp"

namespace prog::sym {

auto operator<<(std::ostream& out, const TypeKind& rhs) -> std::ostream& {
  switch (rhs) {
  case TypeKind::Intrinsic:
    out << "intrinsic";
    break;
  }
  return out;
}

} // namespace prog::sym
