#include "parse/node.hpp"

namespace parse {

auto operator<<(std::ostream& out, const Node& rhs) -> std::ostream& { return rhs.print(out); }

} // namespace parse
