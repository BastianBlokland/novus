#include "prog/sym/func_kind.hpp"

namespace prog::sym {

auto operator<<(std::ostream& out, const FuncKind& rhs) -> std::ostream& {
  switch (rhs) {
  case FuncKind::Intrinsic:
    out << "intrinsic";
    break;
  case FuncKind::User:
    out << "user";
    break;
  }
  return out;
}

} // namespace prog::sym
