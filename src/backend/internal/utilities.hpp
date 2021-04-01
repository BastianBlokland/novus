#pragma once
#include "prog/program.hpp"
#include "prog/sym/const_id.hpp"
#include "prog/sym/field_id.hpp"
#include "prog/sym/func_id.hpp"
#include "prog/sym/type_id.hpp"
#include <algorithm>
#include <string>

namespace backend::internal {

// Get a label to identify the function.
auto getLabel(const prog::Program& prog, prog::sym::FuncId funcId) -> std::string;

// Get a label to identify the (generated) equality function for the user type.
auto getUserTypeEqLabel(const prog::Program& prog, prog::sym::TypeId typeId) -> std::string;

// Get the offset in the constant-table for the constant.
auto getConstOffset(const prog::sym::ConstDeclTable& table, prog::sym::ConstId id) -> uint16_t;

// Get the offset the field has in the struct.
auto getFieldOffset(prog::sym::FieldId fieldId) -> uint8_t;

// Get the discriminant (identifier) of a type in a union.
auto getUnionDiscriminant(
    const prog::Program& prog, prog::sym::TypeId unionType, prog::sym::TypeId targetType)
    -> uint8_t;

// Is the struct a tag-type (struct with no fields).
auto structIsTagType(const prog::Program& prog, prog::sym::TypeId structType) -> bool;

// Is union eligible for the nullable-struct optimisation.
// To be eligible it needs to have 2 entries, a struct with fields and an empty struct.
auto unionIsNullableStruct(const prog::Program& prog, prog::sym::TypeId unionType) -> bool;

template <typename Operation>
auto forEachFuncDef(const prog::Program& program, bool deterministic, Operation op) -> void {
  if (deterministic) {
    auto funcIds = std::vector<prog::sym::FuncId>{};
    for (auto itr = program.beginFuncDefs(); itr != program.endFuncDefs(); ++itr) {
      funcIds.push_back(itr->first);
    }
    std::sort(funcIds.begin(), funcIds.end());
    for (const auto& funcId : funcIds) {
      op(program.getFuncDef(funcId));
    }
  } else {
    for (auto itr = program.beginFuncDefs(); itr != program.endFuncDefs(); ++itr) {
      op(itr->second);
    }
  }
}

} // namespace backend::internal
