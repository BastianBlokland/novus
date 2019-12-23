#include "type_template_table.hpp"

namespace frontend::internal {

auto TypeTemplateTable::declareStruct(
    Context* context,
    const std::string& name,
    std::vector<std::string> typeSubs,
    const parse::StructDeclStmtNode& n) -> void {

  auto structTemplate = StructTemplate{context, name, std::move(typeSubs), n};
  declareTemplate(&m_structs, name, std::move(structTemplate));
}

auto TypeTemplateTable::declareUnion(
    Context* context,
    const std::string& name,
    std::vector<std::string> typeSubs,
    const parse::UnionDeclStmtNode& n) -> void {

  auto unionTemplate = UnionTemplate{context, name, std::move(typeSubs), n};
  declareTemplate(&m_unions, name, std::move(unionTemplate));
}

auto TypeTemplateTable::hasType(const std::string& name) const -> bool {
  return m_structs.find(name) != m_structs.end() || m_unions.find(name) != m_unions.end();
}

auto TypeTemplateTable::instantiate(const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::optional<const TypeTemplateInst*> {
  const auto structInst = instantiateTemplate(&m_structs, name, typeParams);
  if (structInst) {
    return structInst;
  }
  return instantiateTemplate(&m_unions, name, typeParams);
}

template <typename T>
auto TypeTemplateTable::declareTemplate(
    std::unordered_map<std::string, T>* templates, const std::string& name, T newTemplate) -> void {
  if (!templates->insert({name, std::move(newTemplate)}).second) {
    throw std::logic_error{"Template with an identical name has already been declared"};
  }
}

template <typename T>
auto TypeTemplateTable::instantiateTemplate(
    std::unordered_map<std::string, T>* templates,
    const std::string& name,
    const prog::sym::TypeSet& typeParams) const -> std::optional<const TypeTemplateInst*> {

  auto itr = templates->find(name);
  if (itr == templates->end()) {
    return std::nullopt;
  }
  if (itr->second.getTypeParamCount() != typeParams.getCount()) {
    return std::nullopt;
  }
  return itr->second.instantiate(typeParams);
}

} // namespace frontend::internal
