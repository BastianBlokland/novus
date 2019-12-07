#include "prog/sym/type_kind.hpp"

namespace prog::sym {

auto operator<<(std::ostream& out, const TypeKind& rhs) -> std::ostream& {
  switch (rhs) {
  case TypeKind::Bool:
    out << "bool";
    break;
  case TypeKind::Int:
    out << "int";
    break;
  case TypeKind::Float:
    out << "float";
    break;
  case TypeKind::String:
    out << "string";
    break;
  case TypeKind::UserStruct:
    out << "struct";
    break;
  case TypeKind::UserUnion:
    out << "union";
    break;
  }
  return out;
}

} // namespace prog::sym
