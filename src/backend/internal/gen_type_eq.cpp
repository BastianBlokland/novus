#include "gen_type_eq.hpp"
#include "gen_lazy.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <set>

namespace backend::internal {

static auto genTypeEqualityEntry(
    novasm::Assembler* asmb, const prog::Program& prog, const prog::sym::TypeDecl& typeDecl)
    -> void {

  /* 2 values to equate are pushed on the stack before calling this. */

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
  case prog::sym::TypeKind::Future: {
    /* To equate future values we block until we get the result of both and then equate the
    results.*/

    // 'Get' the value for the top-most future.
    asmb->addFutureBlock();

    // 'Swap' the value we just got from the topmost future with the future below it on the stack.
    asmb->addSwap();

    // 'Get' the value for the other future (which is now on the top of the stack).
    asmb->addFutureBlock();

    // At this both values are on the stack, now we can generate the equality check for the result
    // type of the future.
    const auto& futureDef = std::get<prog::sym::FutureDef>(prog.getTypeDef(typeDecl.getId()));
    genTypeEqualityEntry(asmb, prog, prog.getTypeDecl(futureDef.getResult()));
  } break;

  case prog::sym::TypeKind::Lazy: {
    /* For lazy values we 'get' the result of both and then equate their values. */

    // 'Get' the value for the top-most lazy.
    genLazyGet(asmb);

    // 'Swap' the value we just got from the topmost lazy with the lazy below it on the stack.
    asmb->addSwap();

    // 'Get' the value for the other lazy (which is now on the top of the stack).
    genLazyGet(asmb);

    // At this both values are on the stack, now we can generate the equality check for the result
    // type of the lazy.
    const auto& lazyDef = std::get<prog::sym::LazyDef>(prog.getTypeDef(typeDecl.getId()));
    genTypeEqualityEntry(asmb, prog, prog.getTypeDecl(lazyDef.getResult()));
  } break;

  case prog::sym::TypeKind::Struct:
  case prog::sym::TypeKind::Union:
    asmb->addCall(getUserTypeEqLabel(prog, typeDecl.getId()), 2, novasm::CallMode::Normal);
    break;
  case prog::sym::TypeKind::StaticInt:  // By definition static-ints are equal.
  case prog::sym::TypeKind::SysProcess: // Processes are not equality checked at the moment.
  case prog::sym::TypeKind::SysStream:  // Streams are not equality checked at the moment.
    asmb->addLoadLitInt(1);
    break;
  }
}

// This assumes that the structs are stored as consts 0 and 1.
static auto genStructFieldEquality(
    novasm::Assembler* asmb,
    const prog::Program& prog,
    uint8_t fieldOffset,
    const prog::sym::TypeDecl& typeDecl,
    const std::string& eqLabel) {

  // Load the field for both structs on the stack.
  asmb->addStackLoad(0);
  asmb->addStructLoadField(fieldOffset);
  asmb->addStackLoad(1);
  asmb->addStructLoadField(fieldOffset);

  // Check if the field is equal.
  genTypeEqualityEntry(asmb, prog, typeDecl);

  // Jump to the given label if the fields are equal.
  asmb->addJumpIf(eqLabel);
}

static auto genStructEquality(
    novasm::Assembler* asmb, const prog::Program& prog, const prog::sym::StructDef& structDef) {
  asmb->label(getUserTypeEqLabel(prog, structDef.getId()));

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

    const auto& fieldTypeDecl = prog.getTypeDecl(structDef.getFields().begin()->getType());
    genTypeEqualityEntry(asmb, prog, fieldTypeDecl);
    asmb->addRet();
    return;
  }

  const auto& fields = structDef.getFields();
  for (const auto& field : fields) {
    const auto fieldOffset    = getFieldOffset(field.getId());
    const auto& fieldTypeDecl = prog.getTypeDecl(field.getType());

    const auto eqLabel = asmb->generateLabel("struct-field-equal");
    genStructFieldEquality(asmb, prog, fieldOffset, fieldTypeDecl, eqLabel);

    // If not equal return false.
    asmb->addLoadLitInt(0);
    asmb->addRet();

    asmb->label(eqLabel);
  }

  asmb->addLoadLitInt(1);
  asmb->addRet();
}

static auto genNullableStructUnionEquality(
    novasm::Assembler* asmb, const prog::Program& prog, const prog::sym::UnionDef& unionDef) {

  /* For nullable-struct unions we check if the values are both null or if the values are equal. */

  const auto& unionTypes = unionDef.getTypes();
  if (unionTypes.size() != 2) {
    throw std::logic_error{"Nullable-struct unions need to have 2 types"};
  }

  // Find the non-tag struct type in the union.
  const auto structTypeItr =
      std::find_if_not(unionTypes.begin(), unionTypes.end(), [&prog](const auto& id) {
        return structIsTagType(prog, id);
      });
  if (structTypeItr == unionTypes.end()) {
    throw std::logic_error{"No non-tag struct found in union"};
  }

  const auto aNullLabel = asmb->generateLabel("a-null");
  const auto bNullLabel = asmb->generateLabel("b-null");

  // Check if the first nullable struct is null.
  asmb->addStackLoad(0);
  asmb->addCheckStructNull();
  asmb->addJumpIf(aNullLabel);

  // Check if the second nullable struct is null.
  asmb->addStackLoad(1);
  asmb->addCheckStructNull();
  asmb->addJumpIf(bNullLabel);

  // -- Neither a or b is null, we check if the struct is the same.
  asmb->addStackLoad(0);
  asmb->addStackLoad(1);
  // No need for a ret instruction after this as we do a tail-call.
  // Note: we can consider 'inlining' the equality check of the struct here, its a trade-off between
  // code-size and call costs, needs profiling.
  asmb->addCall(getUserTypeEqLabel(prog, *structTypeItr), 2, novasm::CallMode::Tail);

  // -- A is null.
  asmb->label(aNullLabel);

  // If b is also null return 'true' if b is not null return 'false'.
  asmb->addStackLoad(1);
  asmb->addCheckStructNull();
  asmb->addRet();

  // -- B is null, meaning not equal as a was not null.
  asmb->label(bNullLabel);
  asmb->addLoadLitInt(0);
  asmb->addRet();
}

static auto genUnionEquality(
    novasm::Assembler* asmb, const prog::Program& prog, const prog::sym::UnionDef& unionDef) {
  asmb->label(getUserTypeEqLabel(prog, unionDef.getId()));

  if (unionIsNullableStruct(prog, unionDef.getId())) {
    genNullableStructUnionEquality(asmb, prog, unionDef);
    return;
  }

  /*
  For normal unions we check that the discriminant is the same then we check which of the possible
  types it is and then use the appropriate equality logic for that type.
  */

  // Generate some labels to use in this function.
  const auto sameTypeLabel  = asmb->generateLabel("union-same-type");
  const auto sameValueLabel = asmb->generateLabel("union-same-value");
  auto typeLabels           = std::vector<std::string>{};
  typeLabels.reserve(unionDef.getTypes().size());
  for (const auto& unionType : unionDef.getTypes()) {
    const auto& unionTypeName = prog.getTypeDecl(unionType).getName();
    typeLabels.push_back(asmb->generateLabel("union-type-" + unionTypeName));
  }

  // Check if the type (field 0) matches.
  asmb->addStackLoad(0);
  asmb->addStructLoadField(0);
  asmb->addStackLoad(1);
  asmb->addStructLoadField(0);
  asmb->addCheckEqInt();
  asmb->addJumpIf(sameTypeLabel);

  // If types do not match return false.
  asmb->addLoadLitInt(0);
  asmb->addRet();

  // Check which type the union is.
  asmb->label(sameTypeLabel);
  for (auto i = 0U; i != unionDef.getTypes().size(); ++i) {
    asmb->addStackLoad(0);
    asmb->addStructLoadField(0);
    asmb->addLoadLitInt(i);
    asmb->addCheckEqInt();

    // If its this type then jump to the value check for that type.
    asmb->addJumpIf(typeLabels[i]);
  }

  // If the union is none of the types its able to hold then terminate the program.
  asmb->addFail();

  // Check if the value is equal.
  for (auto i = 0U; i != unionDef.getTypes().size(); ++i) {
    const auto& typeDecl = prog.getTypeDecl(unionDef.getTypes()[i]);

    asmb->label(typeLabels[i]);
    genStructFieldEquality(asmb, prog, 1, typeDecl, sameValueLabel);

    // If values do not match return false.
    asmb->addLoadLitInt(0);
    asmb->addRet();
  }

  asmb->label(sameValueLabel);
  asmb->addLoadLitInt(1);
  asmb->addRet();
}

static auto addTypeAndNestedTypes(
    const prog::Program& prog, std::set<prog::sym::TypeId>* set, prog::sym::TypeId id) -> void {

  if (set->insert(id).second) {
    switch (prog.getTypeDecl(id).getKind()) {
    case prog::sym::TypeKind::Struct: {
      for (const auto& f : std::get<prog::sym::StructDef>(prog.getTypeDef(id)).getFields()) {
        addTypeAndNestedTypes(prog, set, f.getType());
      }
    } break;
    case prog::sym::TypeKind::Union: {
      for (const auto& t : std::get<prog::sym::UnionDef>(prog.getTypeDef(id)).getTypes()) {
        addTypeAndNestedTypes(prog, set, t);
      }
    } break;
    default:
      break;
    }
  }
}

auto genUserTypeEquality(novasm::Assembler* asmb, const prog::Program& prog) -> void {
  auto types = std::set<prog::sym::TypeId>{};

  // Gather all types to generate equality functions for.
  for (auto fItr = prog.beginFuncDecls(); fItr != prog.endFuncDecls(); ++fItr) {
    using fk  = prog::sym::FuncKind;
    auto kind = fItr->second.getKind();
    if (kind == fk::CheckEqUserType) {
      addTypeAndNestedTypes(prog, &types, *fItr->second.getInput().begin());
    }
  }

  // Generate equality functions for supported types.
  for (auto typeId : types) {
    switch (prog.getTypeDecl(typeId).getKind()) {
    case prog::sym::TypeKind::Struct: {
      const auto& structDef = std::get<prog::sym::StructDef>(prog.getTypeDef(typeId));
      genStructEquality(asmb, prog, structDef);
    } break;
    case prog::sym::TypeKind::Union: {
      const auto& unionDef = std::get<prog::sym::UnionDef>(prog.getTypeDef(typeId));
      genUnionEquality(asmb, prog, unionDef);
    } break;
    default:
      break;
    }
  }
}

} // namespace backend::internal
