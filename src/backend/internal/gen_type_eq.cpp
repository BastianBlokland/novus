#include "gen_type_eq.hpp"
#include "utilities.hpp"

namespace backend::internal {

// This assumes that the structs are stored as consts 0 and 1.
static auto generateStructFieldEquality(
    Builder* builder,
    uint8_t fieldId,
    const prog::sym::TypeDecl& typeDecl,
    const std::string& eqLabel) {

  // Load the field for both structs on the stack.
  builder->addLoadConst(0);
  builder->addLoadStructField(fieldId);
  builder->addLoadConst(1);
  builder->addLoadStructField(fieldId);

  // Check if the field is equal.
  switch (typeDecl.getKind()) {
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
  case prog::sym::TypeKind::UserUnion:
  case prog::sym::TypeKind::UserDelegate:
    builder->addCall(getUserTypeEqLabel(typeDecl.getId()));
    break;
  }

  // Jump to the given label if the fields are equal.
  builder->addJumpIf(eqLabel);
}

auto generateStructEquality(
    Builder* builder, const prog::Program& program, const prog::sym::StructDef& structDef) -> void {
  builder->label(getUserTypeEqLabel(structDef.getId()));

  // For empty structs we can just return 'true'.
  if (structDef.getFields().getCount() == 0) {
    builder->addPop();
    builder->addPop();
    builder->addLoadLitInt(1);
    builder->addRet();
    builder->addFail(); // Add a fail between sections to aid in detecting invalid programs.
    return;
  }

  // Store the 2 structs as consts.
  builder->addReserveConsts(2);
  builder->addStoreConst(0);
  builder->addStoreConst(1);

  const auto& fields = structDef.getFields();
  for (const auto& field : fields) {
    const auto fieldId        = getFieldId(field.getId());
    const auto& fieldTypeDecl = program.getTypeDecl(field.getType());

    const auto eqLabel = builder->generateLabel();
    generateStructFieldEquality(builder, fieldId, fieldTypeDecl, eqLabel);

    // If not equal return false.
    builder->addLoadLitInt(0);
    builder->addRet();

    builder->label(eqLabel);
  }

  builder->addLoadLitInt(1);
  builder->addRet();
  builder->addFail(); // Add a fail between sections to aid in detecting invalid programs.
}

auto generateUnionEquality(
    Builder* builder, const prog::Program& program, const prog::sym::UnionDef& unionDef) -> void {
  builder->label(getUserTypeEqLabel(unionDef.getId()));

  // Generate some labels to use in this function.
  const auto sameTypeLabel  = builder->generateLabel();
  const auto sameValueLabel = builder->generateLabel();
  auto typeLabels           = std::vector<std::string>{};
  typeLabels.reserve(unionDef.getTypes().size());
  for (auto i = 0U; i != unionDef.getTypes().size(); ++i) {
    typeLabels.push_back(builder->generateLabel());
  }

  // Store the 2 unions as consts.
  builder->addReserveConsts(2);
  builder->addStoreConst(0);
  builder->addStoreConst(1);

  // Check if the type (field 0) matches.
  builder->addLoadConst(0);
  builder->addLoadStructField(0);
  builder->addLoadConst(1);
  builder->addLoadStructField(0);
  builder->addCheckEqInt();
  builder->addJumpIf(sameTypeLabel);

  // If types do not match return false.
  builder->addLoadLitInt(0);
  builder->addRet();

  // Check which type the union is.
  builder->label(sameTypeLabel);
  for (auto i = 0U; i != unionDef.getTypes().size(); ++i) {
    builder->addLoadConst(0);
    builder->addLoadStructField(0);
    builder->addLoadLitInt(i);
    builder->addCheckEqInt();

    // If its this type then jump to the value check for that type.
    builder->addJumpIf(typeLabels[i]);
  }

  // If the union is none of the types its able to hold then terminate the program.
  builder->addFail();

  // Check if the value is equal.
  for (auto i = 0U; i != unionDef.getTypes().size(); ++i) {
    const auto& typeDecl = program.getTypeDecl(unionDef.getTypes()[i]);

    builder->label(typeLabels[i]);
    generateStructFieldEquality(builder, 1, typeDecl, sameValueLabel);

    // If values do not match return false.
    builder->addLoadLitInt(0);
    builder->addRet();
  }

  builder->label(sameValueLabel);
  builder->addLoadLitInt(1);
  builder->addRet();
  builder->addFail(); // Add a fail between sections to aid in detecting invalid programs.
}

auto generateDelegateEquality(
    Builder* builder, const prog::Program& /* unused */, const prog::sym::DelegateDef &
    /* unused */) -> void {
  // Atm delegates are represented by instruction pointers.
  builder->addCheckEqIp();
}

} // namespace backend::internal
