#include "func_template.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/define_user_funcs.hpp"
#include "internal/typeinfer_expr.hpp"
#include "internal/utilities.hpp"

namespace frontend::internal {

FuncTemplate::FuncTemplate(
    Context* context,
    std::string name,
    std::vector<std::string> typeSubs,
    const parse::FuncDeclStmtNode& parseNode) :
    m_context{context},
    m_name{std::move(name)},
    m_typeSubs{std::move(typeSubs)},
    m_parseNode{parseNode} {

  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto FuncTemplate::getTemplateName() const -> const std::string& { return m_name; }

auto FuncTemplate::getTypeParamCount() const -> unsigned int { return m_typeSubs.size(); }

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

  const auto subTable = createSubTable(typeParams);
  auto funcInput      = getFuncInput(m_context, &subTable, m_parseNode);
  if (!funcInput) {
    return std::nullopt;
  }
  auto retType = ::frontend::internal::getRetType(m_context, &subTable, m_parseNode);
  if (!retType) {
    return std::nullopt;
  }
  if (retType->isInfer()) {
    retType = inferRetType(m_context, &subTable, m_parseNode, *funcInput, true);
  }
  return retType->isConcrete() ? std::optional{*retType} : std::nullopt;
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

  m_instances.push_back(std::make_unique<FuncTemplateInst>(FuncTemplateInst{typeParams}));
  auto instance = m_instances.back().get();

  setupInstance(instance);
  return instance;
}

auto FuncTemplate::setupInstance(FuncTemplateInst* instance) -> void {
  const auto subTable = createSubTable(instance->m_typeParams);
  auto funcInput      = getFuncInput(m_context, &subTable, m_parseNode);
  if (!funcInput) {
    assert(m_context->hasErrors());
    return;
  }

  instance->m_retType = ::frontend::internal::getRetType(m_context, &subTable, m_parseNode);
  if (!instance->m_retType) {
    assert(m_context->hasErrors());
    return;
  }

  if (instance->m_retType->isInfer()) {
    instance->m_retType = inferRetType(m_context, &subTable, m_parseNode, *funcInput, true);
    if (!instance->m_retType->isConcrete()) {
      m_context->reportDiag(errUnableToInferFuncReturnType(
          m_context->getSrc(), m_name, m_parseNode.getId().getSpan()));
      return;
    }
  }

  const auto retTypeName = getName(m_context, *instance->m_retType);
  const auto isConv      = isConversion(m_context, m_name);

  // For conversions verify that a correct type is returned.
  if (isConv) {
    // Verify that a conversion to a non-templated type returns the correct type.
    const auto nonTemplConvType = m_context->getProg()->lookupType(m_name);
    if (nonTemplConvType && instance->m_retType != *nonTemplConvType) {
      m_context->reportDiag(errIncorrectReturnTypeInConvFunc(
          m_context->getSrc(), m_name, retTypeName, m_parseNode.getSpan()));
      return;
    }
    // Verify that a conversion to a templated type returns the correct type.
    if (m_context->getTypeTemplates()->hasType(m_name)) {
      const auto typeInfo = m_context->getTypeInfo(*instance->m_retType);
      if (!typeInfo || typeInfo->getName() != m_name) {
        m_context->reportDiag(errIncorrectReturnTypeInConvFunc(
            m_context->getSrc(), m_name, retTypeName, m_parseNode.getSpan()));
        return;
      }
    }
  }

  const auto funcName =
      isConv ? retTypeName : mangleName(m_context, m_name, instance->m_typeParams);

  // Check if an identical function has already been registered.
  if (m_context->getProg()->lookupFunc(funcName, *funcInput, 0)) {
    m_context->reportDiag(
        errDuplicateFuncDeclaration(m_context->getSrc(), m_name, m_parseNode.getSpan()));
    return;
  }

  // Declare the function in the program.
  instance->m_func =
      m_context->getProg()->declareUserFunc(funcName, std::move(*funcInput), *instance->m_retType);

  // Define the function.
  auto defineFuncs = DefineUserFuncs{m_context, &subTable};
  defineFuncs.define(*instance->m_func, m_parseNode);
}

auto FuncTemplate::createSubTable(const prog::sym::TypeSet& typeParams) const
    -> TypeSubstitutionTable {
  auto subTable = TypeSubstitutionTable{};
  for (auto i = 0U; i != m_typeSubs.size(); ++i) {
    subTable.declare(m_typeSubs[i], typeParams[i]);
  }
  return subTable;
}

} // namespace frontend::internal
