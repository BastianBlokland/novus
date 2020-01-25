#include "struct_template.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/define_user_types.hpp"
#include "internal/type_info.hpp"
#include "internal/typeinfer_typesub.hpp"
#include "internal/utilities.hpp"
#include <cassert>

namespace frontend::internal {

StructTemplate::StructTemplate(
    Context* ctx,
    std::string name,
    std::vector<std::string> typeSubs,
    const parse::StructDeclStmtNode& parseNode) :
    TypeTemplateBase{ctx, std::move(name), std::move(typeSubs)}, m_parseNode{parseNode} {}

auto StructTemplate::inferTypeParams(const prog::sym::TypeSet& constructorArgTypes)
    -> std::optional<prog::sym::TypeSet> {
  if (constructorArgTypes.getCount() != m_parseNode.getFields().size()) {
    return std::nullopt;
  }
  auto typeParams = std::vector<prog::sym::TypeId>{};
  for (const auto& typeSub : getTypeSubs()) {
    const auto inferredType =
        inferSubTypeFromSpecs(*getCtx(), typeSub, m_parseNode.getFields(), constructorArgTypes);
    if (!inferredType) {
      return std::nullopt;
    }
    typeParams.push_back(*inferredType);
  }
  return prog::sym::TypeSet{std::move(typeParams)};
}

auto StructTemplate::setupInstance(TypeTemplateInst* instance) -> void {
  const auto mangledName = mangleName(getCtx(), getTemplateName(), instance->m_typeParams);
  const auto subTable    = createSubTable(instance->m_typeParams);

  // Should be unique in the program.
  assert(getCtx()->getProg()->lookupType(mangledName) == std::nullopt);

  // Declare the struct in the program.
  instance->m_type = getCtx()->getProg()->declareStruct(mangledName);

  // Define the struct.
  instance->m_success = defineType(getCtx(), &subTable, *instance->m_type, m_parseNode);

  // Keep track of some extra information about the type.
  getCtx()->declareTypeInfo(
      *instance->m_type,
      TypeInfo{getCtx(), getTemplateName(), m_parseNode.getSpan(), instance->getTypeParams()});
}

} // namespace frontend::internal
