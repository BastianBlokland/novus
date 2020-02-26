#include "gen_type_eq.hpp"
#include "utilities.hpp"

namespace backend::internal {

static auto genTypeEquality(
    novasm::Assembler* asmb, const prog::Program& program, const prog::sym::TypeDecl& typeDecl) {

  switch (typeDecl.getKind()) {
  case prog::sym::TypeKind::Bool:
  case prog::sym::TypeKind::Int:
  case prog::sym::TypeKind::Char:
  case prog::sym::TypeKind::Enum:
    asmb->addCheckEqInt();
    break;
  case prog::sym::TypeKind::Long:
    asmb->addCheckEqLong();
    break;
  case prog::sym::TypeKind::Float:
    asmb->addCheckEqFloat();
    break;
  case prog::sym::TypeKind::String:
    asmb->addCheckEqString();
    break;
  case prog::sym::TypeKind::Delegate:
    asmb->addCheckEqCallDynTgt();
    break;
  case prog::sym::TypeKind::Future:
    // Futures are not equality checked at the moment. A possible implementation would be to wait
    // for both futures to complete and then compare the results but a user probably does not expect
    // a equality check to block.
    asmb->addLoadLitInt(1);
    break;
  case prog::sym::TypeKind::Struct:
  case prog::sym::TypeKind::Union:
    asmb->addCall(getUserTypeEqLabel(program, typeDecl.getId()), 2, novasm::CallMode::Normal);
    break;
  }
}

// This assumes that the structs are stored as consts 0 and 1.
static auto generateStructFieldEquality(
    novasm::Assembler* asmb,
    const prog::Program& program,
    uint8_t fieldId,
    const prog::sym::TypeDecl& typeDecl,
    const std::string& eqLabel) {

  // Load the field for both structs on the stack.
  asmb->addStackLoad(0);
  asmb->addLoadStructField(fieldId);
  asmb->addStackLoad(1);
  asmb->addLoadStructField(fieldId);

  // Check if the field is equal.
  genTypeEquality(asmb, program, typeDecl);

  // Jump to the given label if the fields are equal.
  asmb->addJumpIf(eqLabel);
}

auto generateStructEquality(
    novasm::Assembler* asmb, const prog::Program& program, const prog::sym::StructDef& structDef)
    -> void {
  asmb->label(getUserTypeEqLabel(program, structDef.getId()));

  // For empty structs we can just return 'true'.
  if (structDef.getFields().getCount() == 0U) {
    asmb->addLoadLitInt(1);
    asmb->addRet();
    return;
  }

  // Structs with one field are represented just by the field.
  if (structDef.getFields().getCount() == 1U) {
    // Load both inputs on the stack.
    asmb->addStackLoad(0);
    asmb->addStackLoad(1);

    const auto& fieldTypeDecl = program.getTypeDecl(structDef.getFields().begin()->getType());
    genTypeEquality(asmb, program, fieldTypeDecl);
    asmb->addRet();
    return;
  }

  const auto& fields = structDef.getFields();
  for (const auto& field : fields) {
    const auto fieldId        = getFieldId(field.getId());
    const auto& fieldTypeDecl = program.getTypeDecl(field.getType());

    const auto eqLabel = asmb->generateLabel("struct-field-equal");
    generateStructFieldEquality(asmb, program, fieldId, fieldTypeDecl, eqLabel);

    // If not equal return false.
    asmb->addLoadLitInt(0);
    asmb->addRet();

    asmb->label(eqLabel);
  }

  asmb->addLoadLitInt(1);
  asmb->addRet();
}

auto generateUnionEquality(
    novasm::Assembler* asmb, const prog::Program& program, const prog::sym::UnionDef& unionDef)
    -> void {
  asmb->label(getUserTypeEqLabel(program, unionDef.getId()));

  // Generate some labels to use in this function.
  const auto sameTypeLabel  = asmb->generateLabel("union-same-type");
  const auto sameValueLabel = asmb->generateLabel("union-same-value");
  auto typeLabels           = std::vector<std::string>{};
  typeLabels.reserve(unionDef.getTypes().size());
  for (const auto& unionType : unionDef.getTypes()) {
    const auto& unionTypeName = program.getTypeDecl(unionType).getName();
    typeLabels.push_back(asmb->generateLabel("union-type-" + unionTypeName));
  }

  // Check if the type (field 0) matches.
  asmb->addStackLoad(0);
  asmb->addLoadStructField(0);
  asmb->addStackLoad(1);
  asmb->addLoadStructField(0);
  asmb->addCheckEqInt();
  asmb->addJumpIf(sameTypeLabel);

  // If types do not match return false.
  asmb->addLoadLitInt(0);
  asmb->addRet();

  // Check which type the union is.
  asmb->label(sameTypeLabel);
  for (auto i = 0U; i != unionDef.getTypes().size(); ++i) {
    asmb->addStackLoad(0);
    asmb->addLoadStructField(0);
    asmb->addLoadLitInt(i);
    asmb->addCheckEqInt();

    // If its this type then jump to the value check for that type.
    asmb->addJumpIf(typeLabels[i]);
  }

  // If the union is none of the types its able to hold then terminate the program.
  asmb->addFail();

  // Check if the value is equal.
  for (auto i = 0U; i != unionDef.getTypes().size(); ++i) {
    const auto& typeDecl = program.getTypeDecl(unionDef.getTypes()[i]);

    asmb->label(typeLabels[i]);
    generateStructFieldEquality(asmb, program, 1, typeDecl, sameValueLabel);

    // If values do not match return false.
    asmb->addLoadLitInt(0);
    asmb->addRet();
  }

  asmb->label(sameValueLabel);
  asmb->addLoadLitInt(1);
  asmb->addRet();
}

} // namespace backend::internal
