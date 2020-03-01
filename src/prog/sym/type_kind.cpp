#include "prog/sym/type_kind.hpp"

namespace prog::sym {

auto isPrimitive(const TypeKind& kind) -> bool {
  switch (kind) {
  case TypeKind::Bool:
  case TypeKind::Int:
  case TypeKind::Long:
  case TypeKind::Float:
  case TypeKind::String:
  case TypeKind::Char:
  case TypeKind::Stream:
    return true;
  case TypeKind::Struct:
  case TypeKind::Union:
  case TypeKind::Enum:
  case TypeKind::Delegate:
  case TypeKind::Future:
    return false;
  }
  throw std::invalid_argument{"Unknown type-kind"};
}

auto operator<<(std::ostream& out, const TypeKind& rhs) -> std::ostream& {
  switch (rhs) {
  case TypeKind::Bool:
    out << "bool";
    break;
  case TypeKind::Int:
    out << "int";
    break;
  case TypeKind::Long:
    out << "long";
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
  case TypeKind::Stream:
    out << "stream";
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
  case TypeKind::Future:
    out << "future";
    break;
  }
  return out;
}

} // namespace prog::sym
