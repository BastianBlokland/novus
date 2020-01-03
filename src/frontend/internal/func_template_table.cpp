#include "func_template_table.hpp"

namespace frontend::internal {

auto FuncTemplateTable::hasFunc(const std::string& name) -> bool {
  return m_templates.find(name) != m_templates.end();
}

auto FuncTemplateTable::declare(
    Context* context,
    const std::string& name,
    std::vector<std::string> typeSubs,
    const parse::FuncDeclStmtNode& n) -> void {
  auto funcTemplate = FuncTemplate{context, name, std::move(typeSubs), n};

  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    itr = m_templates.insert({name, std::vector<FuncTemplate>{}}).first;
  }
  itr->second.push_back(std::move(funcTemplate));
}

auto FuncTemplateTable::instantiate(const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::vector<const FuncTemplateInst*> {

  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    return {};
  }

  auto result = std::vector<const FuncTemplateInst*>{};
  for (auto& funcTemplate : itr->second) {
    if (funcTemplate.getTypeParamCount() == typeParams.getCount()) {
      result.push_back(funcTemplate.instantiate(typeParams));
    }
  }
  return result;
}

auto FuncTemplateTable::inferParamsAndInstantiate(
    const std::string& name, const prog::sym::TypeSet& argTypes)
    -> std::vector<const FuncTemplateInst*> {

  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    return {};
  }
  auto result = std::vector<const FuncTemplateInst*>{};
  for (auto& funcTemplate : itr->second) {
    const auto inferredTypeParams = funcTemplate.inferTypeParams(argTypes);
    if (inferredTypeParams) {
      result.push_back(funcTemplate.instantiate(*inferredTypeParams));
    }
  }
  return result;
}

auto FuncTemplateTable::getRetType(const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::optional<prog::sym::TypeId> {
  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    return std::nullopt;
  }

  // Only return a value if all overloads agree on the result-type.
  std::optional<prog::sym::TypeId> result = std::nullopt;
  for (auto& funcTemplate : itr->second) {
    if (funcTemplate.getTypeParamCount() == typeParams.getCount()) {
      const auto retType = funcTemplate.getRetType(typeParams);
      if (!retType) {
        continue;
      }
      if (!result) {
        result = retType;
      } else if (*result != *retType) {
        return std::nullopt;
      }
    }
  }
  return result;
}

auto FuncTemplateTable::inferParamsAndGetRetType(
    const std::string& name, const prog::sym::TypeSet& argTypes)
    -> std::optional<prog::sym::TypeId> {
  auto itr = m_templates.find(name);
  if (itr == m_templates.end()) {
    return std::nullopt;
  }

  // Only return a value if all overloads agree on the result-type.
  std::optional<prog::sym::TypeId> result = std::nullopt;
  for (auto& funcTemplate : itr->second) {
    if (funcTemplate.getArgumentCount() == argTypes.getCount()) {
      const auto inferredTypeParams = funcTemplate.inferTypeParams(argTypes);
      if (inferredTypeParams) {
        const auto retType = funcTemplate.getRetType(*inferredTypeParams);
        if (!retType) {
          continue;
        }
        if (!result) {
          result = retType;
        } else if (*result != *retType) {
          return std::nullopt;
        }
      }
    }
  }
  return result;
}

} // namespace frontend::internal
