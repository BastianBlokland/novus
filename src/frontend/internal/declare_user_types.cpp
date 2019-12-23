#include "internal/declare_user_types.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DeclareUserTypes::DeclareUserTypes(Context* context) : m_context{context} {
  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto DeclareUserTypes::getStructs() const noexcept -> const std::vector<StructDeclarationInfo>& {
  return m_structs;
}

auto DeclareUserTypes::getUnions() const noexcept -> const std::vector<UnionDeclarationInfo>& {
  return m_unions;
}

auto DeclareUserTypes::visit(const parse::StructDeclStmtNode& n) -> void {
  // Get type name.
  const auto name = getName(n.getId());
  if (!validateTypeName(n.getId())) {
    return;
  }

  // If the type is a template then we don't declare it in the program yet but declare it in
  // the type-template table.
  if (n.getTypeSubs()) {
    auto typeSubs = getSubstitutionParams(m_context, *n.getTypeSubs());
    if (typeSubs) {
      m_context->getTypeTemplates()->declareStruct(m_context, name, std::move(*typeSubs), n);
    }
    return;
  }

  // Declare the struct in the program.
  auto typeId = m_context->getProg()->declareUserStruct(name);
  m_structs.emplace_back(typeId, n);

  // Keep track of some extra information about the type.
  m_context->declareTypeInfo(typeId, TypeInfo{name, n.getSpan()});
}

auto DeclareUserTypes::visit(const parse::UnionDeclStmtNode& n) -> void {
  // Get type name.
  const auto name = getName(n.getId());
  if (!validateTypeName(n.getId())) {
    return;
  }

  // If the type is a template then we don't declare it in the program yet but declare it in
  // the type-template table.
  if (n.getTypeSubs()) {
    auto typeSubs = getSubstitutionParams(m_context, *n.getTypeSubs());
    if (typeSubs) {
      m_context->getTypeTemplates()->declareUnion(m_context, name, std::move(*typeSubs), n);
    }
    return;
  }

  // Declare the union in the program.
  auto typeId = m_context->getProg()->declareUserUnion(name);
  m_unions.emplace_back(typeId, n);

  // Keep track of some extra information about the type.
  m_context->declareTypeInfo(typeId, TypeInfo{name, n.getSpan()});
}

auto DeclareUserTypes::validateTypeName(const lex::Token& nameToken) -> bool {
  const auto name = getName(nameToken);
  if (m_context->getProg()->lookupType(name)) {
    m_context->reportDiag(errTypeAlreadyDeclared(m_context->getSrc(), name, nameToken.getSpan()));
    return false;
  }
  if (m_context->getTypeTemplates()->hasType(name)) {
    m_context->reportDiag(
        errTypeTemplateAlreadyDeclared(m_context->getSrc(), name, nameToken.getSpan()));
    return false;
  }
  if (!m_context->getProg()->lookupFuncs(name).empty()) {
    m_context->reportDiag(
        errTypeNameConflictsWithFunc(m_context->getSrc(), name, nameToken.getSpan()));
    return false;
  }
  if (!m_context->getProg()->lookupActions(name).empty()) {
    m_context->reportDiag(
        errTypeNameConflictsWithAction(m_context->getSrc(), name, nameToken.getSpan()));
    return false;
  }
  return true;
}

} // namespace frontend::internal
