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

  const auto isImplicit = n.hasModifier(lex::Keyword::Implicit);

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
      m_ctx->reportDiag(errNonOverloadableOperator, n.getId().getSpan(), n.getId().str());
      return;
    }
    name        = prog::getFuncName(*op);
    displayName = "operator" + n.getId().str();

    if (n.getArgList().getCount() == 0) {
      m_ctx->reportDiag(errOperatorOverloadWithoutArgs, n.getId().getSpan(), displayName);
      return;
    }
  }

  // If the function is a template then we don't declare it in the program yet but declare it in
  // the function-template table.
  if (n.getTypeSubs()) {
    auto typeSubs = getSubstitutionParams(m_ctx, *n.getTypeSubs());
    if (typeSubs && validateFuncTemplateArgList(n.getArgList(), *typeSubs)) {

      if (isImplicit) {
        // Templated implicit conversion are not supported atm.
        // Fundamentally there is no reason we cannot support this but the need has not come up yet.
        m_ctx->reportDiag(errTemplatedImplicitConversion, n.getId().getSpan());
        return;
      }

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
  const auto numOptInputs = getNumOptInputs(m_ctx, n);

  // Verify that this is not a duplicate declaration.
  if (m_ctx->getProg()->lookupFunc(name, input.value(), prog::OvOptions{0})) {
    m_ctx->reportDiag(errDuplicateFuncDeclaration, n.getSpan(), displayName);
    return;
  }

  // Get return type.
  auto retType = getRetType(m_ctx, nullptr, n.getRetType());
  if (!retType) {
    return;
  }

  const auto isConv = isType(m_ctx, nullptr, name);
  if (isConv && n.isAction()) {
    m_ctx->reportDiag(errNonPureConversion, n.getId().getSpan());
    return;
  }
  if (isImplicit && !isConv) {
    m_ctx->reportDiag(errImplicitNonConv, n.getId().getSpan());
    return;
  }
  if (isImplicit && (input->getCount() != 1 || numOptInputs)) {
    m_ctx->reportDiag(errToManyInputsInImplicitConv, n.getId().getSpan());
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
            n.getId().getSpan(),
            name,
            getDisplayName(*m_ctx, *retType));
        return;
      }
    } else {
      if (retType->isInfer()) {
        retType =
            inferRetType(m_ctx, nullptr, n, *input, nullptr, TypeInferExpr::Flags::Aggressive);
        if (!retType->isConcrete()) {
          m_ctx->reportDiag(
              errUnableToInferReturnTypeOfConversionToTemplatedType, n.getId().getSpan(), name);
          return;
        }
      }
      const auto typeInfo = m_ctx->getTypeInfo(*retType);
      if (!typeInfo || typeInfo->getName() != name) {
        m_ctx->reportDiag(
            errIncorrectReturnTypeInConvFunc,
            n.getId().getSpan(),
            name,
            getDisplayName(*m_ctx, *retType));
        return;
      }
    }
  }

  // Declare the function in the program.
  if (n.isAction()) {
    const auto funcId =
        m_ctx->getProg()->declareAction(name, input.value(), retType.value(), numOptInputs);
    m_funcDecls->emplace_back(funcId, m_ctx, std::move(name), n);
    return;
  }
  if (isImplicit) {
    const auto funcId = m_ctx->getProg()->declareImplicitConv((*input)[0], retType.value());
    m_funcDecls->emplace_back(funcId, m_ctx, std::move(name), n);
    return;
  }
  auto funcName     = isConv ? getName(*m_ctx, *retType) : name;
  const auto funcId = m_ctx->getProg()->declarePureFunc(
      std::move(funcName), input.value(), retType.value(), numOptInputs);
  m_funcDecls->emplace_back(funcId, m_ctx, std::move(name), n);
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
  if (!isType(m_ctx, nullptr, name) &&
      std::find(typeSubParams.begin(), typeSubParams.end(), name) == typeSubParams.end()) {

    m_ctx->reportDiag(errUndeclaredType, type.getSpan(), name, type.getParamCount());
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
