#pragma once
#include "internal/context.hpp"
#include <unordered_set>

namespace frontend::internal {

class TypeInferUserFuncs final {
public:
  TypeInferUserFuncs() = delete;
  explicit TypeInferUserFuncs(const TypeSubstitutionTable* typeSubTable);

  auto inferRetType(Context* ctx, prog::sym::FuncId id, const parse::FuncDeclStmtNode& n) -> bool;

private:
  const TypeSubstitutionTable* m_typeSubTable;
  std::unordered_set<prog::sym::FuncId, prog::sym::FuncIdHasher> m_processed;
};

} // namespace frontend::internal
