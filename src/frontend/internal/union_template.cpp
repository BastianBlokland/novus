#include "union_template.hpp"
#include "internal/define_user_types.hpp"
#include "internal/typeinfer_typesub.hpp"
#include "internal/utilities.hpp"
#include <cassert>

namespace frontend::internal {

UnionTemplate::UnionTemplate(
    Context* ctx,
    std::string name,
    std::vector<std::string> typeSubs,
    const parse::UnionDeclStmtNode& parseNode) :
    TypeTemplateBase{ctx, std::move(name), std::move(typeSubs)}, m_parseNode{parseNode} {}

auto UnionTemplate::inferTypeParams(const prog::sym::TypeSet& constructorArgTypes)
    -> std::optional<prog::sym::TypeSet> {
  // Union always consists of 1 value.
  if (constructorArgTypes.getCount() != 1) {
    return std::nullopt;
  }

  auto typeParams = std::vector<prog::sym::TypeId>{};
  for (const auto& typeSub : getTypeSubs()) {
    const auto inferredType = inferSubType(typeSub, constructorArgTypes[0]);
    if (!inferredType) {
      return std::nullopt;
    }
    typeParams.push_back(*inferredType);
  }
  return prog::sym::TypeSet{std::move(typeParams)};
}

auto UnionTemplate::setupInstance(TypeTemplateInst* instance) -> void {
  const auto mangledName = mangleName(getCtx(), getTemplateName(), instance->m_typeParams);
  const auto subTable    = createSubTable(instance->m_typeParams);

  // Should be unique in the program.
  assert(getCtx()->getProg()->lookupType(mangledName) == std::nullopt);

  // Declare the struct in the program.
  instance->m_type = getCtx()->getProg()->declareUnion(mangledName);

  // Define the union.
  instance->m_success = defineType(getCtx(), &subTable, *instance->m_type, m_parseNode);

  // Keep track of some extra information about the type.
  getCtx()->declareTypeInfo(
      *instance->m_type,
      TypeInfo{getCtx(), getTemplateName(), m_parseNode.getSpan(), instance->getTypeParams()});
}

auto UnionTemplate::inferSubType(const std::string& subType, const prog::sym::TypeId& inputType)
    -> std::optional<prog::sym::TypeId> {

  /* Attempt to infer the substituion type by checking in which of the specfied types of the union
  the substitution is used and attempt to resolve that with the input type. Because we don't know
  which of the union options the input type was meant to create we try all of them.  */

  std::optional<prog::sym::TypeId> result = std::nullopt;
  for (const auto& typeSpec : m_parseNode.getTypes()) {
    for (const auto& path : getPathsToTypeSub(subType, typeSpec, {})) {
      const auto& inferredType =
          resolvePathToTypeSub(*getCtx(), path.begin(), path.end(), inputType);
      if (!inferredType) {
        return std::nullopt;
      }
      if (!result) {
        result = inferredType;
      } else if (result != inferredType) {
        return std::nullopt;
      }
    }
  }
  return result;
}

} // namespace frontend::internal
