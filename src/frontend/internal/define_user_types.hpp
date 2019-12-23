#pragma once
#include "internal/context.hpp"
#include "internal/struct_template.hpp"

namespace frontend::internal {

class DefineUserTypes final {
public:
  DefineUserTypes() = delete;
  DefineUserTypes(Context* context, const TypeSubstitutionTable* typeSubTable);

  auto define(prog::sym::TypeId id, const parse::StructDeclStmtNode& n) -> void;
  auto define(prog::sym::TypeId id, const parse::UnionDeclStmtNode& n) -> void;

private:
  Context* m_context;
  const TypeSubstitutionTable* m_typeSubTable;
};

} // namespace frontend::internal
