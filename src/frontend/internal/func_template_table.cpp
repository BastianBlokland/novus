#include "func_template_table.hpp"

namespace frontend::internal {

FuncTemplateTable::FuncTemplateTable(const Source& src, prog::Program* prog) :
    m_src{src}, m_prog{prog} {
  if (m_prog == nullptr) {
    throw std::invalid_argument{"Program cannot be null"};
  }
}

auto FuncTemplateTable::declare(
    const std::string& name, std::vector<std::string> typeParams, const parse::FuncDeclStmtNode& n)
    -> void {
  auto funcTemplate = FuncTemplate{m_src, m_prog, this, name, std::move(typeParams), n};

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

} // namespace frontend::internal
