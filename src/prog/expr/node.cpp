#include "prog/expr/node.hpp"

namespace prog::expr {

Node::Node(NodeKind kind) { m_kind = kind; }

auto Node::getKind() const -> NodeKind { return m_kind; }

auto operator<<(std::ostream& out, const Node& rhs) -> std::ostream& {
  return out << rhs.toString();
}

} // namespace prog::expr
