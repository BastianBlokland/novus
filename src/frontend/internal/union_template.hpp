#pragma once
#include "internal/type_template_base.hpp"
#include "parse/node_stmt_union_decl.hpp"

namespace frontend::internal {

class TypeTemplateTable;

class UnionTemplate final : private TypeTemplateBase {
  friend class TypeTemplateTable;

public:
  using TypeTemplateBase::getTemplateName;
  using TypeTemplateBase::getTypeParamCount;
  using TypeTemplateBase::instantiate;

  UnionTemplate() = delete;

  [[nodiscard]] auto inferTypeParams(const prog::sym::TypeSet& constructorArgTypes)
      -> std::optional<prog::sym::TypeSet> override;

private:
  const parse::UnionDeclStmtNode& m_parseNode;

  UnionTemplate(
      Context* context,
      std::string name,
      std::vector<std::string> typeSubs,
      const parse::UnionDeclStmtNode& parseNode);

  auto setupInstance(TypeTemplateInst* instance) -> void override;

  [[nodiscard]] auto inferSubType(const std::string& subType, const prog::sym::TypeId& inputType)
      -> std::optional<prog::sym::TypeId>;
};

} // namespace frontend::internal
