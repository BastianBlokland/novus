#pragma once
#include "internal/context.hpp"

namespace frontend::internal {

class DefineUserTypes final {
public:
  DefineUserTypes() = delete;
  DefineUserTypes(Context* context, const TypeSubstitutionTable* typeSubTable);

  auto define(prog::sym::TypeId id, const parse::StructDeclStmtNode& n) -> bool;
  auto define(prog::sym::TypeId id, const parse::UnionDeclStmtNode& n) -> bool;
  auto define(prog::sym::TypeId id, const parse::EnumDeclStmtNode& n) -> bool;

private:
  Context* m_context;
  const TypeSubstitutionTable* m_typeSubTable;
};

} // namespace frontend::internal
