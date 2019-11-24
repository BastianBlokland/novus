#include "prog/expr/node.hpp"

namespace prog::expr {

auto operator<<(std::ostream& out, const Node& rhs) -> std::ostream& {
  return out << rhs.toString();
}

} // namespace prog::expr
