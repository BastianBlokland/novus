#include "internal/define_user_types.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include <unordered_map>
#include <unordered_set>

namespace frontend::internal {

DefineUserTypes::DefineUserTypes(Context* context, const TypeSubstitutionTable* typeSubTable) :
    m_context{context}, m_typeSubTable{typeSubTable} {

  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto DefineUserTypes::define(prog::sym::TypeId id, const parse::StructDeclStmtNode& n) -> bool {
  auto isValid    = true;
  auto fieldTable = prog::sym::FieldDeclTable{};
  for (const auto& field : n.getFields()) {
    // Get field type.
    const auto& fieldParseType = field.getType();
    const auto fieldTypeName   = getName(fieldParseType);
    const auto fieldType       = getOrInstType(m_context, m_typeSubTable, fieldParseType);
    if (!fieldType) {
      m_context->reportDiag(errUndeclaredType(
          m_context->getSrc(),
          getName(fieldParseType),
          fieldParseType.getParamCount(),
          fieldParseType.getSpan()));
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
    if (m_context->getProg()->lookupType(fieldName) || isReservedTypeName(fieldName)) {
      m_context->reportDiag(errFieldNameConflictsWithType(
          m_context->getSrc(), fieldName, field.getIdentifier().getSpan()));
      isValid = false;
      continue;
    }
    fieldTable.registerField(fieldName, *fieldType);
  }

  if (isValid) {
    m_context->getProg()->defineStruct(id, std::move(fieldTable));
  }
  return isValid;
}

auto DefineUserTypes::define(prog::sym::TypeId id, const parse::UnionDeclStmtNode& n) -> bool {
  auto isValid = true;
  auto types   = std::vector<prog::sym::TypeId>{};
  for (const auto& parseType : n.getTypes()) {
    const auto type = getOrInstType(m_context, m_typeSubTable, parseType);
    if (!type) {
      m_context->reportDiag(errUndeclaredType(
          m_context->getSrc(), getName(parseType), parseType.getParamCount(), parseType.getSpan()));
      isValid = false;
      continue;
    }
    if (std::find(types.begin(), types.end(), *type) != types.end()) {
      m_context->reportDiag(errDuplicateTypeInUnion(
          m_context->getSrc(),
          getName(parseType),
          getDisplayName(*m_context, *type),
          parseType.getSpan()));
      isValid = false;
      continue;
    }
    types.push_back(*type);
  }

  if (isValid) {
    m_context->getProg()->defineUnion(id, std::move(types));
  }
  return isValid;
}

auto DefineUserTypes::define(prog::sym::TypeId id, const parse::EnumDeclStmtNode& n) -> bool {
  auto isValid      = true;
  auto values       = std::unordered_set<int32_t>{};
  auto entries      = std::unordered_map<std::string, int32_t>{};
  int32_t lastValue = -1;

  for (const auto& entry : n.getEntries()) {
    const auto name  = getName(entry.getIdentifier());
    const auto value = lastValue =
        entry.getValueSpec() ? entry.getValueSpec()->getValue() : lastValue + 1;

    if (!entries.insert({name, value}).second) {
      m_context->reportDiag(
          errDuplicateEntryNameInEnum(m_context->getSrc(), name, entry.getSpan()));
      isValid = false;
    }
    if (!values.insert(value).second) {
      m_context->reportDiag(
          errDuplicateEntryValueInEnum(m_context->getSrc(), value, entry.getSpan()));
      isValid = false;
    }
  }

  if (isValid) {
    m_context->getProg()->defineEnum(id, std::move(entries));
  }
  return isValid;
}

} // namespace frontend::internal
