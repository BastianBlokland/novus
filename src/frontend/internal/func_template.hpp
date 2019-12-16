#pragma once
#include "frontend/source.hpp"
#include "internal/func_template_inst.hpp"
#include "internal/type_substitution_table.hpp"
#include "parse/node_stmt_func_decl.hpp"
#include "prog/program.hpp"

namespace frontend::internal {

class FuncTemplateTable;

class FuncTemplate final {
  friend class FuncTemplateTable;

public:
  FuncTemplate() = delete;

  [[nodiscard]] auto getName() const -> const std::string&;
  [[nodiscard]] auto getTypeParamCount() const -> unsigned int;
  [[nodiscard]] auto getRetType(const prog::sym::TypeSet& typeParams)
      -> std::optional<prog::sym::TypeId>;

  auto instantiate(const prog::sym::TypeSet& typeParams) -> const FuncTemplateInst*;

private:
  const Source& m_src;
  prog::Program* m_prog;
  FuncTemplateTable* m_funcTemplates;
  std::string m_name;
  std::vector<std::string> m_typeSubs;
  const parse::FuncDeclStmtNode& m_parseNode;
  std::vector<std::unique_ptr<FuncTemplateInst>> m_instances;

  FuncTemplate(
      const Source& src,
      prog::Program* prog,
      FuncTemplateTable* funcTemplates,
      std::string name,
      std::vector<std::string> typeSubs,
      const parse::FuncDeclStmtNode& parseNode);

  auto instantiate(FuncTemplateInst* instance) -> void;

  [[nodiscard]] auto mangleName(const prog::sym::TypeSet& typeParams) const -> std::string;

  [[nodiscard]] auto createSubTable(const prog::sym::TypeSet& typeParams) const
      -> TypeSubstitutionTable;

  [[nodiscard]] auto getRetType(
      const TypeSubstitutionTable& subTable,
      const prog::sym::TypeSet& input,
      std::vector<Diag>* diags) const -> std::optional<prog::sym::TypeId>;
};

} // namespace frontend::internal
