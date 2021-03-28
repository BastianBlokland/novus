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
  struct InferResult {
    prog::sym::TypeSet types;
    int complexity;
  };

  FuncTemplate(const FuncTemplate& rhs) = delete;
  FuncTemplate(FuncTemplate&& rhs)      = default;
  ~FuncTemplate()                       = default;

  auto operator=(const FuncTemplate& rhs) -> FuncTemplate& = delete;
  auto operator=(FuncTemplate&& rhs) noexcept -> FuncTemplate& = default;

  [[nodiscard]] auto getTemplateName() const -> const std::string&;
  [[nodiscard]] auto isAction() const -> bool;
  [[nodiscard]] auto getTypeParamCount() const -> unsigned int;

  [[nodiscard]] auto getNumOptArgs() const -> unsigned int;
  [[nodiscard]] auto getMinArgumentCount() const -> unsigned int;
  [[nodiscard]] auto getArgumentCount() const -> unsigned int;
  [[nodiscard]] auto getTemplatedArgumentCount() const -> unsigned int;

  [[nodiscard]] auto getRetType(const prog::sym::TypeSet& typeParams)
      -> std::optional<prog::sym::TypeId>;

  [[nodiscard]] auto getArgumentTypes(const prog::sym::TypeSet& typeParams)
      -> std::optional<prog::sym::TypeSet>;

  [[nodiscard]] auto inferTypeParams(const prog::sym::TypeSet& argTypes)
      -> std::optional<InferResult>;

  [[nodiscard]] auto
  isCallable(const prog::sym::TypeSet& typeParams, const prog::sym::TypeSet& argTypes) -> bool;

  [[nodiscard]] auto instantiate(const prog::sym::TypeSet& typeParams) -> const FuncTemplateInst*;

private:
  Context* m_ctx;
  std::string m_name;
  bool m_isAction;
  std::vector<std::string> m_typeSubs;
  const parse::FuncDeclStmtNode* m_parseNode;
  std::vector<std::unique_ptr<FuncTemplateInst>> m_instances;

  // Keep track of current 'getRetType' calls to detect cycles.
  std::deque<prog::sym::TypeSet> m_inferStack;

  FuncTemplate(
      Context* ctx,
      std::string name,
      bool isAction,
      std::vector<std::string> typeSubs,
      const parse::FuncDeclStmtNode* parseNode);

  auto setupInstance(FuncTemplateInst* instance) -> void;

  [[nodiscard]] auto createSubTable(const prog::sym::TypeSet& typeParams) const
      -> TypeSubstitutionTable;

  // Given a set of argument types, try to infer the types of the remaining (optional) arguments.
  // Returns the full set of arguments (including the given ones) or nullopt.
  [[nodiscard]] auto inferOptArgs(const prog::sym::TypeSet& argTypes)
      -> std::optional<prog::sym::TypeSet>;
};

} // namespace frontend::internal
