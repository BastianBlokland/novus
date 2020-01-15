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
  case TypeKind::Char:
    out << "char";
    break;
  case TypeKind::Struct:
    out << "struct";
    break;
  case TypeKind::Union:
    out << "union";
    break;
  case TypeKind::Enum:
    out << "enum";
    break;
  case TypeKind::Delegate:
    out << "delegate";
    break;
  }
  return out;
}

} // namespace prog::sym
