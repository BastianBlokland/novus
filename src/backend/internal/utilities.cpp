#include "utilities.hpp"
#include <limits>
#include <sstream>

namespace backend::internal {

auto getLabel(prog::sym::FuncId funcId) -> std::string {
  std::ostringstream oss;
  oss << funcId;
  return oss.str();
}

auto getUserTypeEqLabel(prog::sym::TypeId typeId) -> std::string {
  std::ostringstream oss;
  oss << "eq" << typeId;
  return oss.str();
}

auto getConstOffset(const prog::sym::ConstDeclTable& table, prog::sym::ConstId id) -> uint8_t {
  const auto offset = table.getOffset(id);
  if (offset > std::numeric_limits<uint8_t>::max()) {
    throw std::logic_error{"More then 256 constants in one scope are not supported"};
  }
  return static_cast<uint8_t>(offset);
}

auto getFieldId(prog::sym::FieldId fieldId) -> uint8_t {
  auto fieldNum = fieldId.getNum();
  if (fieldNum > std::numeric_limits<uint8_t>::max()) {
    throw std::logic_error{"More then 256 fields in one struct are not supported"};
  }
  return static_cast<uint8_t>(fieldNum);
}

auto getUnionTypeId(
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

} // namespace backend::internal
