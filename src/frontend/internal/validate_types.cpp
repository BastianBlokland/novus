#include "internal/validate_types.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/context.hpp"
#include "prog/sym/struct_def.hpp"
#include "prog/sym/type_id_hasher.hpp"
#include <unordered_set>

namespace frontend::internal {

static auto getCyclicField(
    Context* ctx,
    const prog::sym::FieldDeclTable& fields,
    const std::unordered_set<prog::sym::TypeId, prog::sym::TypeIdHasher>& visitedTypes)
    -> std::optional<prog::sym::FieldId> {

  for (const auto& f : fields) {
    const auto fType = f.getType();
    if (!fType.isConcrete()) {
      continue;
    }
    if (visitedTypes.find(fType) != visitedTypes.end()) {
      return f.getId();
    }
    const auto& fTypeDecl = ctx->getProg()->getTypeDecl(fType);
    if (fTypeDecl.getKind() == prog::sym::TypeKind::Struct) {
      auto childVisitedTypes = visitedTypes;
      childVisitedTypes.insert(fTypeDecl.getId());

      const auto& structDef  = std::get<prog::sym::StructDef>(ctx->getProg()->getTypeDef(fType));
      const auto cyclicField = getCyclicField(ctx, structDef.getFields(), childVisitedTypes);
      if (cyclicField) {
        return f.getId();
      }
    }
  }
  return std::nullopt;
}

auto validateType(prog::sym::TypeId id, const TypeInfo& info) -> void {
  auto* ctx            = info.getCtx();
  const auto& typeDecl = ctx->getProg()->getTypeDecl(id);
  if (typeDecl.getKind() == prog::sym::TypeKind::Struct) {
    const auto& structDef = std::get<prog::sym::StructDef>(ctx->getProg()->getTypeDef(id));
    const auto& fields    = structDef.getFields();

    // Detect cyclic struct.
    auto visitedTypes = std::unordered_set<prog::sym::TypeId, prog::sym::TypeIdHasher>{};
    visitedTypes.insert(id);

    const auto cyclicField = getCyclicField(ctx, fields, visitedTypes);
    if (cyclicField) {
      const auto fieldName = fields[*cyclicField].getName();
      ctx->reportDiag(errCyclicStruct, info.getSourceSpan(), fieldName, typeDecl.getName());
      return;
    }
  }
}

} // namespace frontend::internal
