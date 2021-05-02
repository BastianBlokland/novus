#include "prog/sym/type_kind.hpp"

namespace prog::sym {

auto isPrimitive(const TypeKind& kind) -> bool {
  switch (kind) {
  case TypeKind::Int:
  case TypeKind::Long:
  case TypeKind::Float:
  case TypeKind::Bool:
  case TypeKind::Char:
  case TypeKind::String:
  case TypeKind::SysStream:
  case TypeKind::SysProcess:
  case TypeKind::SysIOWatcher:
    return true;
  case TypeKind::Struct:
  case TypeKind::Union:
  case TypeKind::Enum:
  case TypeKind::Delegate:
  case TypeKind::Future:
  case TypeKind::Lazy:
  case TypeKind::StaticInt:
    return false;
  }
  throw std::invalid_argument{"Unknown type-kind"};
}

auto operator<<(std::ostream& out, const TypeKind& rhs) -> std::ostream& {
  switch (rhs) {
  case TypeKind::Int:
    out << "int";
    break;
  case TypeKind::Long:
    out << "long";
    break;
  case TypeKind::Float:
    out << "float";
    break;
  case TypeKind::Bool:
    out << "bool";
    break;
  case TypeKind::Char:
    out << "char";
    break;
  case TypeKind::String:
    out << "string";
    break;
  case TypeKind::SysStream:
    out << "sys_stream";
    break;
  case TypeKind::SysProcess:
    out << "sys_process";
    break;
  case TypeKind::SysIOWatcher:
    out << "sys_iowatcher";
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
  case TypeKind::Lazy:
    out << "lazy";
    break;
  case TypeKind::StaticInt:
    out << "staticint";
    break;
  }
  return out;
}

} // namespace prog::sym
