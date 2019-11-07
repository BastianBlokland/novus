#include "prog/sym/const_kind.hpp"

namespace prog::sym {

auto operator<<(std::ostream& out, const ConstKind& rhs) -> std::ostream& {
  switch (rhs) {
  case ConstKind::Input:
    out << "in";
    break;
  case ConstKind::Local:
    out << "loc";
    break;
  }
  return out;
}

} // namespace prog::sym
