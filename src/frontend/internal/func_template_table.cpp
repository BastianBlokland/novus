#include "func_template_table.hpp"
#include "typeinfer_typesub.hpp"
#include <limits>

namespace frontend::internal {

auto inline satisfiesOptions(const FuncTemplate& templ, prog::OvOptions options) -> bool {
  if (options.hasFlag<prog::OvFlags::ExclPureFuncs>() && !templ.isAction()) {
    return false;
  }
  if (options.hasFlag<prog::OvFlags::ExclActions>() && templ.isAction()) {
    return false;
  }
  return true;
}

auto FuncTemplateTable::hasFunc(const std::string& name) -> bool {
  return m_templates.find(name) != m_templates.end();
}

auto FuncTemplateTable::declarePure(
    Context* ctx,
    const std::string& name,
    std::vector<std::string> typeSubs,
    const parse::FuncDeclStmtNode* n) -> void {
  declare(ctx, name, false, std::move(typeSubs), n);
}

auto FuncTemplateTable::declareAction(
    Context* ctx,
    const std::string& name,
    std::vector<std::string> typeSubs,
    const parse::FuncDeclStmtNode* n) -> void {
  declare(ctx, name, true, std::move(typeSubs), n);
}

auto FuncTemplateTable::instantiate(
    const std::string& name, const prog::sym::TypeSet& typeParams, prog::OvOptions options)
    -> std::vector<const FuncTemplateInst*> {

  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    return {};
  }

  auto result = std::vector<const FuncTemplateInst*>{};
  for (auto& funcTemplate : itr->second) {
    if (satisfiesOptions(funcTemplate, options) &&
        funcTemplate.getTypeParamCount() == typeParams.getCount()) {
      result.push_back(funcTemplate.instantiate(typeParams));
    }
  }
  return result;
}

auto FuncTemplateTable::inferParamsAndInstantiate(
    const std::string& name, const prog::sym::TypeSet& argTypes, prog::OvOptions options)
    -> std::vector<const FuncTemplateInst*> {

  auto result = std::vector<const FuncTemplateInst*>{};
  for (const auto& funcTemplAndParams : inferParams(name, argTypes, options)) {
    result.push_back(funcTemplAndParams.first->instantiate(funcTemplAndParams.second));
  }
  return result;
}

auto FuncTemplateTable::getCallInfo(
    const std::string& name, const prog::sym::TypeSet& typeParams, prog::OvOptions options)
    -> std::optional<CallInfo> {
  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    return std::nullopt;
  }

  // Only return a value if all overloads agree on the info.
  std::optional<CallInfo> result = std::nullopt;
  for (auto& funcTemplate : itr->second) {
    if (satisfiesOptions(funcTemplate, options) &&
        funcTemplate.getTypeParamCount() == typeParams.getCount()) {

      const auto retType  = funcTemplate.getRetType(typeParams);
      const auto isAction = funcTemplate.isAction();
      if (!retType || retType->isInfer()) {
        continue;
      }
      if (!result) {
        result = {*retType, isAction};
      } else if (result->resultType != *retType || result->isAction != isAction) {
        return std::nullopt;
      }
    }
  }
  return result;
}

auto FuncTemplateTable::inferParamsAndGetCallInfo(
    const std::string& name, const prog::sym::TypeSet& argTypes, prog::OvOptions options)
    -> std::optional<CallInfo> {

  // Only return a value if all templates agree on the info.
  std::optional<CallInfo> result = std::nullopt;
  for (const auto& funcTemplAndParams : inferParams(name, argTypes, options)) {
    const auto retType  = funcTemplAndParams.first->getRetType(funcTemplAndParams.second);
    const auto isAction = funcTemplAndParams.first->isAction();
    if (!retType || retType->isInfer()) {
      continue;
    }
    if (!result) {
      result = {*retType, isAction};
    } else if (result->resultType != *retType || result->isAction != isAction) {
      return std::nullopt;
    }
  }
  return result;
}

auto FuncTemplateTable::declare(
    Context* ctx,
    const std::string& name,
    bool isAction,
    std::vector<std::string> typeSubs,
    const parse::FuncDeclStmtNode* n) -> void {

  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    itr = m_templates.insert({name, std::vector<FuncTemplate>{}}).first;
  }
  itr->second.push_back(FuncTemplate{ctx, name, isAction, std::move(typeSubs), n});
}

auto FuncTemplateTable::inferParams(
    const std::string& name, const prog::sym::TypeSet& argTypes, prog::OvOptions options)
    -> std::vector<std::pair<FuncTemplate*, prog::sym::TypeSet>> {

  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    return {};
  }

  /* Find the templates where we can successfully infer the type-parameters (and call it with the
  given argument types), prefer templates with lower complexity type-parameters. Can return
  multiple templates if they have the same complexity. */

  auto typeParamComplexity = std::numeric_limits<int>::max();
  auto result              = std::vector<std::pair<FuncTemplate*, prog::sym::TypeSet>>{};

  for (auto& funcTemplate : itr->second) {
    if (argTypes.getCount() < funcTemplate.getMinArgumentCount() ||
        argTypes.getCount() > funcTemplate.getArgumentCount()) {
      continue;
    }
    if (options.hasFlag<prog::OvFlags::NoOptArgs>() &&
        argTypes.getCount() != funcTemplate.getArgumentCount()) {
      continue;
    }

    if (satisfiesOptions(funcTemplate, options)) {

      const auto inferResult = funcTemplate.inferTypeParams(argTypes);
      if (inferResult && funcTemplate.isCallable(inferResult->types, argTypes)) {

        if (inferResult->complexity < typeParamComplexity) {
          result.clear();
          result.emplace_back(&funcTemplate, inferResult->types);
          typeParamComplexity = inferResult->complexity;
        } else if (inferResult->complexity == typeParamComplexity) {
          result.emplace_back(&funcTemplate, inferResult->types);
        }
      }
    }
  }

  return result;
}

} // namespace frontend::internal
