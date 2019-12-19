#include "internal/define_user_types.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DefineUserTypes::DefineUserTypes(Context* context, const TypeSubstitutionTable* typeSubTable) :
    m_context{context}, m_typeSubTable{typeSubTable} {

  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto DefineUserTypes::define(prog::sym::TypeId id, const parse::StructDeclStmtNode& n) -> void {
  auto isValid    = true;
  auto fieldTable = prog::sym::FieldDeclTable{};
  for (const auto& field : n.getFields()) {
    // Get field type.
    const auto fieldTypeName = getName(field.getType());
    const auto fieldType     = getOrInstType(m_context, m_typeSubTable, field.getType());
    if (!fieldType) {
      if (!m_context->hasErrors()) {
        m_context->reportDiag(errUndeclaredType(
            m_context->getSrc(), getName(field.getType()), field.getType().getSpan()));
      }
      isValid = false;
      continue;
    }

    // Get field identifier.
    const auto fieldName = getName(field.getIdentifier());
    if (fieldTable.lookup(fieldName)) {
      m_context->reportDiag(errDuplicateFieldNameInStruct(
          m_context->getSrc(), fieldName, field.getIdentifier().getSpan()));
      isValid = false;
      continue;
    }
    if (m_typeSubTable != nullptr && m_typeSubTable->lookupType(fieldName)) {
      m_context->reportDiag(errFieldNameConflictsWithTypeSubstitution(
          m_context->getSrc(), fieldName, field.getIdentifier().getSpan()));
      isValid = false;
      continue;
    }
    if (m_context->getProg()->lookupType(fieldName)) {
      m_context->reportDiag(errFieldNameConflictsWithType(
          m_context->getSrc(), fieldName, field.getIdentifier().getSpan()));
      isValid = false;
      continue;
    }
    fieldTable.registerField(fieldName, *fieldType);
  }

  if (isValid) {
    m_context->getProg()->defineUserStruct(id, std::move(fieldTable));
  }
}

auto DefineUserTypes::define(prog::sym::TypeId id, const parse::UnionDeclStmtNode& n) -> void {
  auto isValid = true;
  auto types   = std::vector<prog::sym::TypeId>{};
  for (const auto& parseType : n.getTypes()) {
    const auto type = getOrInstType(m_context, m_typeSubTable, parseType);
    if (!type) {
      m_context->reportDiag(
          errUndeclaredType(m_context->getSrc(), getName(parseType), parseType.getSpan()));
      isValid = false;
      continue;
    }
    if (std::find(types.begin(), types.end(), *type) != types.end()) {
      m_context->reportDiag(
          errDuplicateTypeInUnion(m_context->getSrc(), getName(parseType), parseType.getSpan()));
      isValid = false;
      continue;
    }
    types.push_back(*type);
  }

  if (isValid) {
    m_context->getProg()->defineUserUnion(id, std::move(types));
  }
}

} // namespace frontend::internal
