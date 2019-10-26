#include "prog/sym/const_kind.hpp"

namespace prog::sym {

auto operator<<(std::ostream& out, const ConstKind& rhs) -> std::ostream& {
  switch (rhs) {
  case ConstKind::Input:
    out << "input";
    break;
  case ConstKind::Local:
    out << "local";
    break;
  }
  return out;
}

} // namespace prog::sym
