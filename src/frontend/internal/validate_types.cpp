#include "internal/validate_types.hpp"
#include "frontend/diag_defs.hpp"

namespace frontend::internal {

ValidateTypes::ValidateTypes(Context* context) : m_context{context} {
  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto ValidateTypes::validate(prog::sym::TypeId id) -> void {
  const auto& typedecl = m_context->getProg()->getTypeDecl(id);
  if (typedecl.getKind() == prog::sym::TypeKind::UserStruct) {
    const auto typeInfo   = m_context->getTypeInfo(id);
    const auto sourceSpan = typeInfo ? typeInfo->getSourceSpan() : input::Span{0, 0};
    const auto& structDef = std::get<prog::sym::StructDef>(m_context->getProg()->getTypeDef(id));
    const auto& fields    = structDef.getFields();

    // Detect cyclic struct.
    auto visitedTypes = std::unordered_set<prog::sym::TypeId, prog::sym::TypeIdHasher>{};
    visitedTypes.insert(id);

    const auto cyclicField = getCyclicField(fields, &visitedTypes);
    if (cyclicField) {
      const auto fieldName = fields[*cyclicField].getName();
      m_context->reportDiag(
          errCyclicStruct(m_context->getSrc(), fieldName, typedecl.getName(), sourceSpan));
      return;
    }
  }
}

auto ValidateTypes::getCyclicField(
    const prog::sym::FieldDeclTable& fields,
    std::unordered_set<prog::sym::TypeId, prog::sym::TypeIdHasher>* visitedTypes)
    -> std::optional<prog::sym::FieldId> {

  for (const auto& f : fields) {
    const auto fType = f.getType();
    if (visitedTypes->find(fType) != visitedTypes->end()) {
      return f.getId();
    }
    const auto& fTypeDecl = m_context->getProg()->getTypeDecl(fType);
    if (fTypeDecl.getKind() == prog::sym::TypeKind::UserStruct) {
      visitedTypes->insert(fTypeDecl.getId());

      const auto& structDef =
          std::get<prog::sym::StructDef>(m_context->getProg()->getTypeDef(fType));
      const auto cyclicField = getCyclicField(structDef.getFields(), visitedTypes);
      if (cyclicField) {
        return f.getId();
      }
    }
  }
  return std::nullopt;
}

} // namespace frontend::internal
