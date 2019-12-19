#include "internal/declare_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DeclareUserFuncs::DeclareUserFuncs(Context* context) : m_context{context} {
  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto DeclareUserFuncs::getFuncs() const noexcept -> const std::vector<DeclarationInfo>& {
  return m_funcs;
}

auto DeclareUserFuncs::visit(const parse::FuncDeclStmtNode& n) -> void {
  std::string name;
  std::string displayName;
  if (n.getId().getKind() == lex::TokenKind::Identifier) {
    name = displayName = getName(n.getId());
    if (!validateFuncName(n.getId())) {
      return;
    }
  } else {
    auto op = getOperator(n.getId());
    if (!op) {
      m_context->reportDiag(
          errNonOverloadableOperator(m_context->getSrc(), n.getId().str(), n.getId().getSpan()));
      return;
    }
    name        = prog::getFuncName(*op);
    displayName = "operator-" + n.getId().str();
  }

  // If the function is a template then we don't declare it in the program yet but declare it in
  // the function-template table.
  if (n.getTypeSubs()) {
    auto typeSubs = getSubstitutionParams(m_context, *n.getTypeSubs());
    if (typeSubs) {
      m_context->getFuncTemplates()->declare(m_context, name, std::move(*typeSubs), n);
    }
    return;
  }

  // Get func input.
  const auto input = getFuncInput(m_context, nullptr, n);
  if (!input) {
    return;
  }

  // Verify that this is not a duplicate declaration.
  if (m_context->getProg()->lookupFunc(name, input.value(), 0)) {
    m_context->reportDiag(
        errDuplicateFuncDeclaration(m_context->getSrc(), displayName, n.getSpan()));
    return;
  }

  // Get return type.
  auto retType = getRetType(m_context, nullptr, n);
  if (!retType) {
    return;
  }

  // Check if this function is a conversion.
  const auto convType = m_context->getProg()->lookupType(name);
  if (convType) {
    if (retType->isInfer()) {
      retType = convType;
    } else if (*retType != *convType) {
      m_context->reportDiag(errIncorrectReturnTypeInConvFunc(
          m_context->getSrc(), name, getName(m_context, *retType), n.getId().getSpan()));
    }
  }

  // Declare the function in the program.
  auto funcId = m_context->getProg()->declareUserFunc(name, input.value(), retType.value());
  m_funcs.emplace_back(funcId, n);
}

auto DeclareUserFuncs::validateFuncName(const lex::Token& nameToken) -> bool {
  const auto name = getName(nameToken);
  if (!m_context->getProg()->lookupActions(name).empty()) {
    m_context->reportDiag(
        errFuncNameConflictsWithAction(m_context->getSrc(), name, nameToken.getSpan()));
    return false;
  }
  return true;
}

} // namespace frontend::internal
