#include "utilities.hpp"
#include <limits>
#include <sstream>

namespace backend::internal {

auto getLabel(const prog::Program& prog, prog::sym::FuncId funcId) -> std::string {
  std::ostringstream oss;
  oss << "func_" + prog.getFuncDecl(funcId).getName() << "_" << funcId;
  return oss.str();
}

auto getUserTypeEqLabel(const prog::Program& prog, prog::sym::TypeId typeId) -> std::string {
  std::ostringstream oss;
  oss << "user_type_equality_" << prog.getTypeDecl(typeId).getName() << "_" << typeId;
  return oss.str();
}

auto getConstOffset(const prog::sym::ConstDeclTable& table, prog::sym::ConstId id) -> uint8_t {
  const auto offset = table.getOffset(id);
  if (offset > std::numeric_limits<uint8_t>::max()) {
    throw std::logic_error{"More then 256 constants in one scope are not supported"};
  }
  return static_cast<uint8_t>(offset);
}

auto getFieldOffset(prog::sym::FieldId fieldId) -> uint8_t {
  auto fieldNum = fieldId.getNum();
  if (fieldNum > std::numeric_limits<uint8_t>::max()) {
    throw std::logic_error{"More then 256 fields in one struct are not supported"};
  }
  return static_cast<uint8_t>(fieldNum);
}

auto getUnionDiscriminant(
    const prog::Program& prog, prog::sym::TypeId unionType, prog::sym::TypeId targetType)
    -> uint8_t {
  const auto& typeDef = prog.getTypeDef(unionType);
  if (!std::holds_alternative<prog::sym::UnionDef>(typeDef)) {
    throw std::logic_error{"Child expression has to be of a union type"};
  }
  const auto& unionDef = std::get<prog::sym::UnionDef>(typeDef);
  const auto index     = unionDef.getTypeIndex(targetType);
  if (index > std::numeric_limits<uint8_t>::max()) {
    throw std::logic_error{"More then 256 types in one union are not supported"};
  }
  return static_cast<uint8_t>(index);
}

auto structIsTagType(const prog::Program& prog, prog::sym::TypeId structType) -> bool {
  const auto& typeDecl = prog.getTypeDecl(structType);
  if (typeDecl.getKind() != prog::sym::TypeKind::Struct) {
    throw std::logic_error{"Given type is not a struct"};
  }
  return std::get<prog::sym::StructDef>(prog.getTypeDef(structType)).isTagType();
}

auto unionIsNullableStruct(const prog::Program& prog, prog::sym::TypeId unionType) -> bool {
  /*
  To be eligible for the nullable struct optimisation it needs to have 2 entries:
    * a struct with fields (2 or more), note: struct with 1 field is not eligible as backend is
      allowed to represent 1 field strusts just by the field itself.
    * a empty struct (sometimes called a tag type).
  In case of nullable structs the union can be optimized away and just having a struct that is
  'null' in case of the empty struct and non 'null' in case of the struct with fields.
  */

  const auto& typeDecl = prog.getTypeDecl(unionType);
  if (typeDecl.getKind() != prog::sym::TypeKind::Union) {
    throw std::logic_error{"Given type is not a union"};
  }
  const auto& unionDef = std::get<prog::sym::UnionDef>(prog.getTypeDef(unionType));
  const auto& types    = unionDef.getTypes();

  // Needs to have two types.
  if (types.size() != 2) {
    return false;
  }

  // Both types need to be structs.
  if (!std::all_of(types.begin(), types.end(), [&prog](const auto& id) {
        return prog.getTypeDecl(id).getKind() == prog::sym::TypeKind::Struct;
      })) {
    return false;
  }

  // One struct needs to be a tag type (having no fields).
  if (!std::any_of(types.begin(), types.end(), [&prog](const auto& id) {
        return structIsTagType(prog, id);
      })) {
    return false;
  }

  // One struct needs to have 2 or more fields.
  if (!std::any_of(types.begin(), types.end(), [&prog](const auto& id) {
        const auto& structDef = std::get<prog::sym::StructDef>(prog.getTypeDef(id));
        return structDef.getFields().getCount() >= 2;
      })) {
    return false;
  }

  // Union is eligible.
  return true;
}

} // namespace backend::internal
