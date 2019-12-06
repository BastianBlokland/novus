#include "gen_type_eq.hpp"
#include "utilities.hpp"

namespace backend::internal {

auto generateStructEquality(
    Builder* builder, const prog::Program& program, const prog::sym::StructDef& structDef) -> void {
  builder->label(getUserTypeEqLabel(structDef.getId()));

  // Store the 2 struct's as consts.
  builder->addReserveConsts(2);
  builder->addStoreConst(0);
  builder->addStoreConst(1);

  const auto& fields = structDef.getFields();
  for (const auto& field : fields) {
    const auto fieldId        = getFieldId(field.getId());
    const auto& fieldTypeDecl = program.getTypeDecl(field.getType());

    // Load the field for both structs on the stack.
    builder->addLoadConst(0);
    builder->addLoadStructField(fieldId);
    builder->addLoadConst(1);
    builder->addLoadStructField(fieldId);

    // Check if the field is equal.
    switch (fieldTypeDecl.getKind()) {
    case prog::sym::TypeKind::Bool:
    case prog::sym::TypeKind::Int:
      builder->addCheckEqInt();
      break;
    case prog::sym::TypeKind::Float:
      builder->addCheckEqFloat();
      break;
    case prog::sym::TypeKind::String:
      builder->addCheckEqString();
      break;
    case prog::sym::TypeKind::UserStruct:
      builder->addCall(getUserTypeEqLabel(field.getType()));
      break;
    }

    // If equal jump to the next field.
    const auto eqLabel = builder->generateLabel();
    builder->addJumpIf(eqLabel);

    // If not equal return false.
    builder->addLoadLitInt(0);
    builder->addRet();

    builder->label(eqLabel);
  }

  builder->addLoadLitInt(1);
  builder->addRet();
  builder->addFail(); // Add a fail between sections to aid in detecting invalid programs.
}

} // namespace backend::internal
