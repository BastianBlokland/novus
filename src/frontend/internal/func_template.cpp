#include "func_template.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/define_user_funcs.hpp"
#include "internal/typeinfer_expr.hpp"
#include "internal/typeinfer_typesub.hpp"
#include "internal/utilities.hpp"
#include "parse/type_param_list.hpp"
#include <cassert>
#include <optional>

namespace frontend::internal {

namespace {

auto isParseTypeTemplated(
    const parse::Type& parseType, const std::vector<std::string>& typeSubs) noexcept {
  const auto typeName = getName(parseType.getId());
  for (const auto& sub : typeSubs) {
    if (typeName == sub) {
      return true;
    }
  }
  if (parseType.getParamList()) {
    for (const auto& param : *parseType.getParamList()) {
      if (isParseTypeTemplated(param, typeSubs)) {
        return true;
      }
    }
  }
  return false;
}

} // namespace

FuncTemplate::FuncTemplate(
    Context* ctx,
    std::string name,
    bool isAction,
    std::vector<std::string> typeSubs,
    const parse::FuncDeclStmtNode* parseNode) :
    m_ctx{ctx},
    m_name{std::move(name)},
    m_isAction{isAction},
    m_typeSubs{std::move(typeSubs)},
    m_parseNode{parseNode} {

  if (m_ctx == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
  if (m_parseNode == nullptr) {
    throw std::invalid_argument{"ParseNode cannot be null"};
  }
}

auto FuncTemplate::getTemplateName() const -> const std::string& { return m_name; }

auto FuncTemplate::isAction() const -> bool { return m_isAction; }

auto FuncTemplate::getTypeParamCount() const -> unsigned int { return m_typeSubs.size(); }

auto FuncTemplate::getNumOptArgs() const -> unsigned int {
  return m_parseNode->getArgList().getInitializerCount();
}

auto FuncTemplate::getMinArgumentCount() const -> unsigned int {
  return getArgumentCount() - getNumOptArgs();
}

auto FuncTemplate::getArgumentCount() const -> unsigned int {
  return m_parseNode->getArgList().getCount();
}

auto FuncTemplate::getTemplatedArgumentCount() const -> unsigned int {
  auto result = 0u;
  for (const auto& parseArg : m_parseNode->getArgList()) {
    if (isParseTypeTemplated(parseArg.getType(), m_typeSubs)) {
      ++result;
    }
  }
  return result;
}

auto FuncTemplate::getRetType(const prog::sym::TypeSet& typeParams)
    -> std::optional<prog::sym::TypeId> {
  if (typeParams.getCount() != m_typeSubs.size()) {
    throw std::invalid_argument{"Invalid number of type-parameters provided"};
  }

  // Check if we have a previous instantiation for the given type params then return that type.
  const auto previousInst =
      std::find_if(m_instances.begin(), m_instances.end(), [&typeParams](const auto& inst) {
        return inst->getTypeParams() == typeParams;
      });
  if (previousInst != m_instances.end()) {
    return (*previousInst)->m_retType;
  }

  // Check if we are already infering these types, prevents infinite loop in case of recursion.
  if (std::find(m_inferStack.begin(), m_inferStack.end(), typeParams) != m_inferStack.end()) {
    return std::nullopt;
  }
  m_inferStack.push_front(typeParams);

  const auto subTable = createSubTable(typeParams);
  auto funcInput      = getFuncInput(m_ctx, &subTable, *m_parseNode);
  if (!funcInput) {
    return std::nullopt;
  }
  auto retType = ::frontend::internal::getRetType(m_ctx, &subTable, m_parseNode->getRetType());
  if (!retType) {
    m_inferStack.pop_front();
    return std::nullopt;
  }
  if (retType->isInfer()) {
    auto inferFlags = TypeInferExpr::Flags::Aggressive;
    if (m_isAction) {
      inferFlags = inferFlags | TypeInferExpr::Flags::AllowActionCalls;
    }
    retType = inferRetType(m_ctx, &subTable, *m_parseNode, *funcInput, nullptr, inferFlags);
  }

  m_inferStack.pop_front();
  return retType->isConcrete() ? std::optional{*retType} : std::nullopt;
}

auto FuncTemplate::getArgumentTypes(const prog::sym::TypeSet& typeParams)
    -> std::optional<prog::sym::TypeSet> {
  const auto subTable = createSubTable(typeParams);
  return getFuncInput(m_ctx, &subTable, *m_parseNode);
}

auto FuncTemplate::inferTypeParams(const prog::sym::TypeSet& argTypes)
    -> std::optional<InferResult> {

  auto inputTypes = inferOptArgs(argTypes);
  if (!inputTypes) {
    return std::nullopt;
  }

  auto typeParams = std::vector<prog::sym::TypeId>{};
  for (const auto& typeSub : m_typeSubs) {
    const auto inferredType =
        inferSubTypeFromSpecs(*m_ctx, typeSub, m_parseNode->getArgList().getArgs(), *inputTypes);
    if (!inferredType || !inferredType->isConcrete()) {
      return std::nullopt;
    }
    typeParams.push_back(*inferredType);
  }

  /* The complexity of an infer result is used for overloaded function templates to determine the
   * best match (with the lowest complexity).
   *
   * The complexity is determined by the amount of templated arguments to the function + the
   * complexity of the inferred substitutions (deeper nested types have higher complexity).
   */

  InferResult result;
  result.types = prog::sym::TypeSet{std::move(typeParams)};
  result.complexity =
      static_cast<int>(getTemplatedArgumentCount()) + getComplexity(*m_ctx, result.types);
  return result;
}

auto FuncTemplate::isCallable(
    const prog::sym::TypeSet& typeParams, const prog::sym::TypeSet& argTypes) -> bool {
  const auto subTable = createSubTable(typeParams);
  auto funcInput      = getFuncInput(m_ctx, &subTable, *m_parseNode);
  return funcInput &&
      m_ctx->getProg()->isImplicitConvertible(*funcInput, argTypes, getNumOptArgs());
}

auto FuncTemplate::instantiate(const prog::sym::TypeSet& typeParams) -> const FuncTemplateInst* {
  if (typeParams.getCount() != m_typeSubs.size()) {
    throw std::invalid_argument{"Invalid number of type-parameters provided"};
  }

  // Check if we have a previous instantiation for the given type params.
  const auto previousInst =
      std::find_if(m_instances.begin(), m_instances.end(), [&typeParams](const auto& inst) {
        return inst->getTypeParams() == typeParams;
      });
  if (previousInst != m_instances.end()) {
    return previousInst->get();
  }

  m_instances.push_back(std::make_unique<FuncTemplateInst>(FuncTemplateInst{m_name, typeParams}));
  auto instance = m_instances.back().get();

  setupInstance(instance);
  return instance;
}

auto FuncTemplate::setupInstance(FuncTemplateInst* instance) -> void {
  const auto subTable = createSubTable(instance->m_typeParams);
  auto funcInput      = getFuncInput(m_ctx, &subTable, *m_parseNode);
  if (!funcInput) {
    assert(m_ctx->hasErrors());
    instance->m_success = false;
    return;
  }
  const auto numOptInputs = getNumOptInputs(m_ctx, *m_parseNode);

  instance->m_retType =
      ::frontend::internal::getRetType(m_ctx, &subTable, m_parseNode->getRetType());
  if (!instance->m_retType) {
    assert(m_ctx->hasErrors());
    instance->m_success = false;
    return;
  }

  const auto isConv = isType(m_ctx, nullptr, m_name);
  if (isConv && m_isAction) {
    m_ctx->reportDiag(errNonPureConversion, m_parseNode->getSpan());
    instance->m_success = false;
    return;
  }

  if (instance->m_retType->isInfer()) {
    // For conversions to non-templated types we know the return-type by looking at the name.
    if (isConv && m_ctx->getProg()->lookupType(m_name)) {
      instance->m_retType = m_ctx->getProg()->lookupType(m_name);
    } else {
      auto inferFlags = TypeInferExpr::Flags::Aggressive;
      if (m_isAction) {
        inferFlags = inferFlags | TypeInferExpr::Flags::AllowActionCalls;
      }

      // Otherwise try to infer the return-type.
      instance->m_retType =
          inferRetType(m_ctx, &subTable, *m_parseNode, *funcInput, nullptr, inferFlags);
      // We don't produce a diagnostic yet if the inferring failed as that is done by the definition
      // step.
    }
  }

  // For conversions verify that a correct type is returned.
  if (isConv) {
    const auto nonTemplConvType = m_ctx->getProg()->lookupType(m_name);
    if (nonTemplConvType) {
      // Verify that a conversion to a non-templated type returns the correct type.
      if (instance->m_retType != *nonTemplConvType) {
        m_ctx->reportDiag(
            errIncorrectReturnTypeInConvFunc,
            m_parseNode->getSpan(),
            instance->getDisplayName(*m_ctx),
            getDisplayName(*m_ctx, *instance->m_retType));

        instance->m_success = false;
        return;
      }
    } else {
      // Verify that a conversion to a templated type returns the correct type.
      const auto typeInfo = m_ctx->getTypeInfo(*instance->m_retType);
      if (!typeInfo || typeInfo->getName() != m_name) {
        m_ctx->reportDiag(
            errIncorrectReturnTypeInConvFunc,
            m_parseNode->getSpan(),
            instance->getDisplayName(*m_ctx),
            getDisplayName(*m_ctx, *instance->m_retType));

        instance->m_success = false;
        return;
      }
    }
  }

  const auto funcName = isConv ? getName(*m_ctx, *instance->m_retType)
                               : mangleName(m_ctx, m_name, instance->m_typeParams);

  // Check if an identical function has already been registered.
  if (m_ctx->getProg()->lookupFunc(funcName, *funcInput, prog::OvOptions{0})) {
    m_ctx->reportDiag(
        errDuplicateFuncDeclaration, m_parseNode->getSpan(), instance->getDisplayName(*m_ctx));
    instance->m_success = false;
    return;
  }

  // Declare the function in the program.
  instance->m_func = m_isAction
      ? m_ctx->getProg()->declareAction(
            funcName, std::move(*funcInput), *instance->m_retType, numOptInputs)
      : m_ctx->getProg()->declarePureFunc(
            funcName, std::move(*funcInput), *instance->m_retType, numOptInputs);

  // Define the function.
  instance->m_success = defineFunc(
      m_ctx, &subTable, *instance->m_func, instance->getDisplayName(*m_ctx), *m_parseNode);

  // If we failed to infer a return-type for this function a diagnostic should have been emitted
  // during definition (the reason for not emitting one before is that otherwise it might hide
  // other errors in the function).
  assert(instance->m_retType->isInfer() ? m_ctx->hasErrors() : true);
}

auto FuncTemplate::createSubTable(const prog::sym::TypeSet& typeParams) const
    -> TypeSubstitutionTable {
  auto subTable = TypeSubstitutionTable{};
  for (auto i = 0U; i != m_typeSubs.size(); ++i) {
    subTable.declare(m_typeSubs[i], typeParams[i]);
  }
  return subTable;
}

auto FuncTemplate::inferOptArgs(const prog::sym::TypeSet& argTypes)
    -> std::optional<prog::sym::TypeSet> {

  const auto maxInputs    = m_parseNode->getArgList().getCount();
  const auto numOptInputs = getNumOptInputs(m_ctx, *m_parseNode);
  const auto minInputs    = maxInputs - numOptInputs;

  if (argTypes.getCount() < minInputs || argTypes.getCount() > maxInputs) {
    return std::nullopt;
  }

  auto inputTypes = std::vector<prog::sym::TypeId>{};
  inputTypes.reserve(maxInputs);

  // Add the provided inputs.
  for (auto argType : argTypes) {
    inputTypes.push_back(argType);
  }

  // Add the types for the optional inputs.
  // TODO: These initializer types could be cached.
  for (auto i = argTypes.getCount() - minInputs; i != numOptInputs; ++i) {

    // Infer the type of the optional arg initializer.
    // NOTE: Will only work if the initializer does not depend on any of the type parameters.
    auto inferFlags = TypeInferExpr::Flags::NoOptArgs;
    if (m_isAction) {
      inferFlags = inferFlags | TypeInferExpr::Flags::AllowActionCalls;
    }
    auto inferInitializerType = TypeInferExpr{m_ctx, nullptr, nullptr, inferFlags};
    m_parseNode->operator[](i).accept(&inferInitializerType);

    // NOTE: Can fail to infer the type, in which case the type is 'inferType'.
    inputTypes.push_back(inferInitializerType.getInferredType());
  }

  return prog::sym::TypeSet{std::move(inputTypes)};
}

} // namespace frontend::internal
