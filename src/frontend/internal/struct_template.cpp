#include "struct_template.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/define_user_types.hpp"
#include "internal/type_info.hpp"
#include "internal/utilities.hpp"

namespace frontend::internal {

StructTemplate::StructTemplate(
    Context* context,
    std::string name,
    std::vector<std::string> typeSubs,
    const parse::StructDeclStmtNode& parseNode) :
    TypeTemplateBase{context, std::move(name), std::move(typeSubs)}, m_parseNode{parseNode} {}

auto StructTemplate::setupInstance(TypeTemplateInst* instance) -> void {
  const auto mangledName = mangleName(getContext(), getTemplateName(), instance->m_typeParams);
  const auto subTable    = createSubTable(instance->m_typeParams);

  // Should be unique in the program.
  assert(getContext()->getProg()->lookupType(mangledName) == std::nullopt);

  // Declare the struct in the program.
  instance->m_type = getContext()->getProg()->declareUserStruct(mangledName);

  // Define the struct.
  auto defineTypes = DefineUserTypes{getContext(), &subTable};
  defineTypes.define(*instance->m_type, m_parseNode);

  // Keep track of some extra information about the type.
  getContext()->declareTypeInfo(
      *instance->m_type,
      TypeInfo{getTemplateName(), m_parseNode.getSpan(), instance->getTypeParams()});
}

} // namespace frontend::internal
