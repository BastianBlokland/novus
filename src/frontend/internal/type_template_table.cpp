#include "type_template_table.hpp"
#include <cassert>

namespace frontend::internal {

auto TypeTemplateTable::declareStruct(
    Context* ctx,
    const std::string& name,
    std::vector<std::string> typeSubs,
    const parse::StructDeclStmtNode& n) -> void {

  auto structTemplate = StructTemplate{ctx, name, std::move(typeSubs), n};
  declareTemplate(&m_structs, name, std::move(structTemplate));
}

auto TypeTemplateTable::declareUnion(
    Context* ctx,
    const std::string& name,
    std::vector<std::string> typeSubs,
    const parse::UnionDeclStmtNode& n) -> void {

  auto unionTemplate = UnionTemplate{ctx, name, std::move(typeSubs), n};
  declareTemplate(&m_unions, name, std::move(unionTemplate));
}

auto TypeTemplateTable::hasType(const std::string& name) -> bool {
  return m_structs.find(name) != m_structs.end() || m_unions.find(name) != m_unions.end();
}

auto TypeTemplateTable::hasType(const std::string& name, unsigned int numTypeParams) -> bool {
  return lookup(&m_structs, name, numTypeParams) || lookup(&m_unions, name, numTypeParams);
}

auto TypeTemplateTable::instantiate(const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::optional<const TypeTemplateInst*> {
  const auto structInst = instantiateTemplate(&m_structs, name, typeParams);
  if (structInst) {
    return structInst;
  }
  return instantiateTemplate(&m_unions, name, typeParams);
}

auto TypeTemplateTable::inferParamsAndInstantiate(
    const std::string& name, const prog::sym::TypeSet& constructorArgTypes)
    -> std::optional<const TypeTemplateInst*> {
  const auto structInst = inferParamsAndInstantiate(&m_structs, name, constructorArgTypes);
  if (structInst) {
    return structInst;
  }
  return inferParamsAndInstantiate(&m_unions, name, constructorArgTypes);
}

template <typename T>
auto TypeTemplateTable::lookup(
    TemplateMap<T>* templates, const std::string& name, unsigned int numTypeParams) -> T* {
  auto itrs = templates->equal_range(name);
  for (; itrs.first != itrs.second; ++itrs.first) {
    if (itrs.first->second.getTypeParamCount() == numTypeParams) {
      return &itrs.first->second;
    }
  }
  return nullptr;
}

template <typename T>
auto TypeTemplateTable::declareTemplate(
    TemplateMap<T>* templates, const std::string& name, T newTemplate) -> void {

  assert(!lookup(templates, name, newTemplate.getTypeParamCount()));
  templates->insert({name, std::move(newTemplate)});
}

template <typename T>
auto TypeTemplateTable::instantiateTemplate(
    TemplateMap<T>* templates, const std::string& name, const prog::sym::TypeSet& typeParams)
    -> std::optional<const TypeTemplateInst*> {

  T* entry = lookup(templates, name, typeParams.getCount());
  if (!entry) {
    return std::nullopt;
  }
  return entry->instantiate(typeParams);
}

template <typename T>
auto TypeTemplateTable::inferParamsAndInstantiate(
    TemplateMap<T>* templates,
    const std::string& name,
    const prog::sym::TypeSet& constructorArgTypes) const -> std::optional<const TypeTemplateInst*> {

  auto itrs = templates->equal_range(name);
  for (; itrs.first != itrs.second; ++itrs.first) {
    T& templ                      = itrs.first->second;
    const auto inferredTypeParams = templ.inferTypeParams(constructorArgTypes);
    if (inferredTypeParams) {
      return templ.instantiate(*inferredTypeParams);
    }
  }
  return std::nullopt;
}

} // namespace frontend::internal
