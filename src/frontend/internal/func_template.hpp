#pragma once
#include "internal/func_template_inst.hpp"
#include "internal/type_substitution_table.hpp"
#include "parse/node_stmt_func_decl.hpp"
#include "prog/program.hpp"
#include <deque>

namespace frontend::internal {

class Context;

class FuncTemplate final {
  friend class FuncTemplateTable;

public:
  FuncTemplate() = delete;

  [[nodiscard]] auto getTemplateName() const -> const std::string&;
  [[nodiscard]] auto getTypeParamCount() const -> unsigned int;
  [[nodiscard]] auto getArgumentCount() const -> unsigned int;
  [[nodiscard]] auto getRetType(const prog::sym::TypeSet& typeParams)
      -> std::optional<prog::sym::TypeId>;

  [[nodiscard]] auto inferTypeParams(const prog::sym::TypeSet& argTypes)
      -> std::optional<prog::sym::TypeSet>;

  [[nodiscard]] auto instantiate(const prog::sym::TypeSet& typeParams) -> const FuncTemplateInst*;

private:
  Context* m_context;
  std::string m_name;
  std::vector<std::string> m_typeSubs;
  const parse::FuncDeclStmtNode& m_parseNode;
  std::vector<std::unique_ptr<FuncTemplateInst>> m_instances;

  // Keep track of current 'getRetType' calls to detect cycles.
  std::deque<prog::sym::TypeSet> m_inferStack;

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
