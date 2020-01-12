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
    displayName = "operator" + n.getId().str();

    if (n.getArgList().getCount() == 0) {
      m_context->reportDiag(
          errOperatorOverloadWithoutArgs(m_context->getSrc(), displayName, n.getId().getSpan()));
      return;
    }
  }

  // If the function is a template then we don't declare it in the program yet but declare it in
  // the function-template table.
  if (n.getTypeSubs()) {
    auto typeSubs = getSubstitutionParams(m_context, *n.getTypeSubs());
    if (typeSubs && validateFuncTemplateArgList(n.getArgList(), *typeSubs)) {
      m_context->getFuncTemplates()->declare(m_context, name, std::move(*typeSubs), &n);
    }
    return;
  }

  // Get func input.
  const auto input = getFuncInput(m_context, nullptr, n);
  if (!input) {
    return;
  }

  // Verify that this is not a duplicate declaration.
  if (m_context->getProg()->lookupFunc(name, input.value(), 0, false)) {
    m_context->reportDiag(
        errDuplicateFuncDeclaration(m_context->getSrc(), displayName, n.getSpan()));
    return;
  }

  // Get return type.
  auto retType = getRetType(m_context, nullptr, n);
  if (!retType) {
    return;
  }

  // For conversions validate that correct types are returned.
  const auto isConv = isType(m_context, name);
  if (isConv) {
    const auto nonTemplConvType = m_context->getProg()->lookupType(name);
    if (nonTemplConvType) {
      if (retType->isInfer()) {
        retType = nonTemplConvType;
      } else if (*retType != *nonTemplConvType) {
        m_context->reportDiag(errIncorrectReturnTypeInConvFunc(
            m_context->getSrc(), name, getDisplayName(*m_context, *retType), n.getId().getSpan()));
        return;
      }
    } else {
      if (retType->isInfer()) {
        retType = inferRetType(m_context, nullptr, n, *input, nullptr, true);
        if (!retType->isConcrete()) {
          m_context->reportDiag(errUnableToInferReturnTypeOfConversionToTemplatedType(
              m_context->getSrc(), name, n.getId().getSpan()));
          return;
        }
      }
      const auto typeInfo = m_context->getTypeInfo(*retType);
      if (!typeInfo || typeInfo->getName() != name) {
        m_context->reportDiag(errIncorrectReturnTypeInConvFunc(
            m_context->getSrc(), name, getDisplayName(*m_context, *retType), n.getId().getSpan()));
        return;
      }
    }
  }

  // Declare the function in the program.
  const auto funcName = isConv ? getName(*m_context, *retType) : name;
  auto funcId = m_context->getProg()->declareUserFunc(funcName, input.value(), retType.value());
  m_funcs.emplace_back(funcId, name, n);
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

auto DeclareUserFuncs::validateFuncTemplateArgList(
    const parse::ArgumentListDecl& args, const std::vector<std::string>& typeSubParams) -> bool {

  auto isValid = true;
  for (const auto& arg : args) {
    isValid &= validateType(arg.getType(), typeSubParams);
  }
  return isValid;
}

auto DeclareUserFuncs::validateType(
    const parse::Type& type, const std::vector<std::string>& typeSubParams) -> bool {

  auto isValid    = true;
  const auto name = getName(type.getId());
  if (!isType(m_context, name) &&
      std::find(typeSubParams.begin(), typeSubParams.end(), name) == typeSubParams.end()) {

    m_context->reportDiag(
        errUndeclaredType(m_context->getSrc(), name, type.getParamCount(), type.getSpan()));
    isValid = false;
  }

  if (type.getParamList()) {
    for (const auto& sub : *type.getParamList()) {
      isValid &= validateType(sub, typeSubParams);
    }
  }
  return isValid;
}

} // namespace frontend::internal
