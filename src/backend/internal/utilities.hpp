#pragma once
#include "prog/program.hpp"
#include "prog/sym/const_id.hpp"
#include "prog/sym/field_id.hpp"
#include "prog/sym/func_id.hpp"
#include "prog/sym/type_id.hpp"
#include <string>

namespace backend::internal {

auto getLabel(prog::sym::FuncId funcId) -> std::string;

auto getUserTypeEqLabel(prog::sym::TypeId typeId) -> std::string;

auto getConstId(prog::sym::ConstId constId) -> uint8_t;

auto getFieldId(prog::sym::FieldId fieldId) -> uint8_t;

auto getUnionTypeId(
    const prog::Program& prog, prog::sym::TypeId unionType, prog::sym::TypeId targetType)
    -> uint8_t;

} // namespace backend::internal
