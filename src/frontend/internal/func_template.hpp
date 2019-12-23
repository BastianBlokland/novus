#pragma once
#include "internal/func_template_inst.hpp"
#include "internal/type_substitution_table.hpp"
#include "parse/node_stmt_func_decl.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

class Context;

class FuncTemplate final {
  friend class FuncTemplateTable;

public:
  FuncTemplate() = delete;

  [[nodiscard]] auto getTemplateName() const -> const std::string&;
  [[nodiscard]] auto getTypeParamCount() const -> unsigned int;
  [[nodiscard]] auto getRetType(const prog::sym::TypeSet& typeParams)
      -> std::optional<prog::sym::TypeId>;

  auto instantiate(const prog::sym::TypeSet& typeParams) -> const FuncTemplateInst*;

private:
  Context* m_context;
  std::string m_name;
  std::vector<std::string> m_typeSubs;
  const parse::FuncDeclStmtNode& m_parseNode;
  std::vector<std::unique_ptr<FuncTemplateInst>> m_instances;

  FuncTemplate(
      Context* context,
      std::string name,
      std::vector<std::string> typeSubs,
      const parse::FuncDeclStmtNode& parseNode);

  auto setupInstance(FuncTemplateInst* instance) -> void;

  [[nodiscard]] auto createSubTable(const prog::sym::TypeSet& typeParams) const
      -> TypeSubstitutionTable;
};

} // namespace frontend::internal
