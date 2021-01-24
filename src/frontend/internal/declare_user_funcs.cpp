#include "internal/declare_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DeclareUserFuncs::DeclareUserFuncs(Context* ctx, std::vector<FuncDeclInfo>* funcDecls) :
    m_ctx{ctx}, m_funcDecls{funcDecls} {

  if (m_ctx == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
  if (m_funcDecls == nullptr) {
    throw std::invalid_argument{"FuncDecl vector cannot be null"};
  }
}

auto DeclareUserFuncs::visit(const parse::FuncDeclStmtNode& n) -> void {

  std::string name;
  std::string displayName;
  if (n.getId().getKind() == lex::TokenKind::Identifier) {
    name = displayName = getName(n.getId());
  } else {
    if (n.isAction()) {
      m_ctx->reportDiag(errNonPureOperatorOverload, n.getId().getSpan());
      return;
    }

    auto op = getOperator(n.getId());
    if (!op) {
      m_ctx->reportDiag(errNonOverloadableOperator, n.getId().str(), n.getId().getSpan());
      return;
    }
    name        = prog::getFuncName(*op);
    displayName = "operator" + n.getId().str();

    if (n.getArgList().getCount() == 0) {
      m_ctx->reportDiag(errOperatorOverloadWithoutArgs, displayName, n.getId().getSpan());
      return;
    }
  }

  // If the function is a template then we don't declare it in the program yet but declare it in
  // the function-template table.
  if (n.getTypeSubs()) {
    auto typeSubs = getSubstitutionParams(m_ctx, *n.getTypeSubs());
    if (typeSubs && validateFuncTemplateArgList(n.getArgList(), *typeSubs)) {

      if (n.isAction()) {
        m_ctx->getFuncTemplates()->declareAction(m_ctx, name, std::move(*typeSubs), &n);
      } else {
        m_ctx->getFuncTemplates()->declarePure(m_ctx, name, std::move(*typeSubs), &n);
      }
    }
    return;
  }

  // Get func input.
  const auto input = getFuncInput(m_ctx, nullptr, n);
  if (!input) {
    return;
  }
  const auto numOptArgs = getNumOptionalArgs(m_ctx, n);
  (void)numOptArgs;

  // Verify that this is not a duplicate declaration.
  if (m_ctx->getProg()->lookupFunc(name, input.value(), prog::OvOptions{0})) {
    m_ctx->reportDiag(errDuplicateFuncDeclaration, displayName, n.getSpan());
    return;
  }

  // Get return type.
  auto retType = getRetType(m_ctx, nullptr, n.getRetType());
  if (!retType) {
    return;
  }

  const auto isConv = isType(m_ctx, name);
  if (isConv && n.isAction()) {
    if (n.isAction()) {
      m_ctx->reportDiag(errNonPureConversion, n.getId().getSpan());
      return;
    }
    return;
  }

  // For conversions validate that correct types are returned.
  if (isConv) {
    const auto nonTemplConvType = m_ctx->getProg()->lookupType(name);
    if (nonTemplConvType) {
      if (retType->isInfer()) {
        retType = nonTemplConvType;
      } else if (*retType != *nonTemplConvType) {
        m_ctx->reportDiag(
            errIncorrectReturnTypeInConvFunc,
            name,
            getDisplayName(*m_ctx, *retType),
            n.getId().getSpan());
        return;
      }
    } else {
      if (retType->isInfer()) {
        retType =
            inferRetType(m_ctx, nullptr, n, *input, nullptr, TypeInferExpr::Flags::Aggressive);
        if (!retType->isConcrete()) {
          m_ctx->reportDiag(
              errUnableToInferReturnTypeOfConversionToTemplatedType, name, n.getId().getSpan());
          return;
        }
      }
      const auto typeInfo = m_ctx->getTypeInfo(*retType);
      if (!typeInfo || typeInfo->getName() != name) {
        m_ctx->reportDiag(
            errIncorrectReturnTypeInConvFunc,
            name,
            getDisplayName(*m_ctx, *retType),
            n.getId().getSpan());
        return;
      }
    }
  }

  // Declare the function in the program.
  const auto funcName = isConv ? getName(*m_ctx, *retType) : name;
  auto funcId         = n.isAction()
      ? m_ctx->getProg()->declareAction(funcName, input.value(), retType.value())
      : m_ctx->getProg()->declarePureFunc(funcName, input.value(), retType.value());
  m_funcDecls->emplace_back(funcId, m_ctx, name, n);
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
  if (!isType(m_ctx, name) &&
      std::find(typeSubParams.begin(), typeSubParams.end(), name) == typeSubParams.end()) {

    m_ctx->reportDiag(errUndeclaredType, name, type.getParamCount(), type.getSpan());
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
