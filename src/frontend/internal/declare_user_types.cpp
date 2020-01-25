#include "internal/declare_user_types.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DeclareUserTypes::DeclareUserTypes(
    Context* ctx,
    std::vector<StructDeclInfo>* structDecls,
    std::vector<UnionDeclInfo>* unionDecls,
    std::vector<EnumDeclInfo>* enumDecls) :
    m_ctx{ctx}, m_structDecls{structDecls}, m_unionDecls{unionDecls}, m_enumDecls{enumDecls} {

  if (m_ctx == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
  if (m_structDecls == nullptr) {
    throw std::invalid_argument{"StructDecl vector cannot be null"};
  }
  if (m_unionDecls == nullptr) {
    throw std::invalid_argument{"UnionDecl vector cannot be null"};
  }
  if (m_enumDecls == nullptr) {
    throw std::invalid_argument{"EnumDecl vector cannot be null"};
  }
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
    auto typeSubs = getSubstitutionParams(m_ctx, *n.getTypeSubs());
    if (typeSubs) {
      m_ctx->getTypeTemplates()->declareStruct(m_ctx, name, std::move(*typeSubs), n);
    }
    return;
  }

  // Declare the struct in the program.
  auto typeId = m_ctx->getProg()->declareStruct(name);
  m_structDecls->emplace_back(typeId, m_ctx, n);

  // Keep track of some extra information about the type.
  m_ctx->declareTypeInfo(typeId, TypeInfo{m_ctx, name, n.getSpan()});
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
    auto typeSubs = getSubstitutionParams(m_ctx, *n.getTypeSubs());
    if (typeSubs) {
      m_ctx->getTypeTemplates()->declareUnion(m_ctx, name, std::move(*typeSubs), n);
    }
    return;
  }

  // Declare the union in the program.
  auto typeId = m_ctx->getProg()->declareUnion(name);
  m_unionDecls->emplace_back(typeId, m_ctx, n);

  // Keep track of some extra information about the type.
  m_ctx->declareTypeInfo(typeId, TypeInfo{m_ctx, name, n.getSpan()});
}

auto DeclareUserTypes::visit(const parse::EnumDeclStmtNode& n) -> void {
  // Get type name.
  const auto name = getName(n.getId());
  if (!validateTypeName(n.getId())) {
    return;
  }

  // Declare the enum in the program.
  auto typeId = m_ctx->getProg()->declareEnum(name);
  m_enumDecls->emplace_back(typeId, m_ctx, n);

  // Keep track of some extra information about the type.
  m_ctx->declareTypeInfo(typeId, TypeInfo{m_ctx, name, n.getSpan()});
}

auto DeclareUserTypes::validateTypeName(const lex::Token& nameToken) -> bool {
  const auto name = getName(nameToken);
  if (m_ctx->getProg()->lookupType(name)) {
    m_ctx->reportDiag(errTypeAlreadyDeclared, name, nameToken.getSpan());
    return false;
  }
  if (m_ctx->getTypeTemplates()->hasType(name)) {
    m_ctx->reportDiag(errTypeTemplateAlreadyDeclared, name, nameToken.getSpan());
    return false;
  }
  if (isReservedTypeName(name)) {
    m_ctx->reportDiag(errTypeNameIsReserved, name, nameToken.getSpan());
    return false;
  }
  if (m_ctx->getProg()->hasFunc(name)) {
    m_ctx->reportDiag(errTypeNameConflictsWithFunc, name, nameToken.getSpan());
    return false;
  }
  return true;
}

} // namespace frontend::internal
