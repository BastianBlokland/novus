#include "prog/expr/node_kind.hpp"

namespace prog::expr {

auto operator<<(std::ostream& out, const NodeKind& rhs) -> std::ostream& {
  switch (rhs) {
  case NodeKind::Group:
    out << "group";
    break;
  case NodeKind::Branch:
    out << "branch";
    break;
  case NodeKind::Assign:
    out << "assign";
    break;
  case NodeKind::Const:
    out << "const";
    break;
  case NodeKind::Call:
    out << "call";
    break;
  case NodeKind::LitInt:
    out << "lit-int";
    break;
  case NodeKind::LitBool:
    out << "lit-bool";
    break;
  }
  return out;
}

} // namespace prog::expr
