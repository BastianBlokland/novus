#pragma once
#include "internal/type_template_base.hpp"
#include "parse/node_stmt_struct_decl.hpp"

namespace frontend::internal {

class Context;
class TypeTemplateTable;

class StructTemplate final : private TypeTemplateBase {
  friend class TypeTemplateTable;

public:
  using TypeTemplateBase::getTemplateName;
  using TypeTemplateBase::getTypeParamCount;
  using TypeTemplateBase::instantiate;

  StructTemplate() = delete;

private:
  const parse::StructDeclStmtNode& m_parseNode;

  StructTemplate(
      Context* context,
      std::string name,
      std::vector<std::string> typeSubs,
      const parse::StructDeclStmtNode& parseNode);

  auto setupInstance(TypeTemplateInst* instance) -> void override;
};

} // namespace frontend::internal
