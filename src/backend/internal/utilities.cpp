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

auto getConstId(prog::sym::ConstId constId) -> uint8_t {
  auto constNum = constId.getNum();
  if (constNum > std::numeric_limits<uint8_t>::max()) {
    throw std::logic_error{"More then 256 constants in one scope are not supported"};
  }
  return static_cast<uint8_t>(constNum);
}

auto getFieldId(prog::sym::FieldId fieldId) -> uint8_t {
  auto fieldNum = fieldId.getNum();
  if (fieldNum > std::numeric_limits<uint8_t>::max()) {
    throw std::logic_error{"More then 256 fields in one struct are not supported"};
  }
  return static_cast<uint8_t>(fieldNum);
}

} // namespace backend::internal
