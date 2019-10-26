#include "prog/sym/action_kind.hpp"

namespace prog::sym {

auto operator<<(std::ostream& out, const ActionKind& rhs) -> std::ostream& {
  switch (rhs) {
  case ActionKind::Intrinsic:
    out << "intrinsic";
    break;
  }
  return out;
}

} // namespace prog::sym
