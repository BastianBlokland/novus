#include "parse/node.hpp"

namespace parse {

auto operator<<(std::ostream& out, const Node& rhs) -> std::ostream& {
  out << rhs.getType();
  out << "-'";
  rhs.print(out);
  out << '\'';
  return out;
}

} // namespace parse
