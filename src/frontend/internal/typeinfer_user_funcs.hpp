#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "internal/func_template_table.hpp"
#include "prog/program.hpp"
#include <unordered_set>

namespace frontend::internal {

class TypeInferUserFuncs final {
public:
  TypeInferUserFuncs() = delete;
  TypeInferUserFuncs(
      prog::Program* prog,
      FuncTemplateTable* funcTemplates,
      const TypeSubstitutionTable* typeSubTable);

  auto inferRetType(prog::sym::FuncId id, const parse::FuncDeclStmtNode& n) -> bool;

private:
  prog::Program* m_prog;
  FuncTemplateTable* m_funcTemplates;
  const TypeSubstitutionTable* m_typeSubTable;
  std::unordered_set<prog::sym::FuncId, prog::sym::FuncIdHasher> m_processed;
};

} // namespace frontend::internal
