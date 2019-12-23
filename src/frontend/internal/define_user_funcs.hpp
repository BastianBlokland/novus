#pragma once
#include "internal/context.hpp"
#include "internal/type_substitution_table.hpp"

namespace frontend::internal {

class DefineUserFuncs final {
public:
  DefineUserFuncs() = delete;
  DefineUserFuncs(Context* context, const TypeSubstitutionTable* typeSubTable);

  auto define(prog::sym::FuncId id, const parse::FuncDeclStmtNode& n) -> void;

private:
  Context* m_context;
  const TypeSubstitutionTable* m_typeSubTable;

  auto declareInputs(const parse::FuncDeclStmtNode& n, prog::sym::ConstDeclTable* consts) -> bool;
  auto getExpr(
      const parse::Node& n,
      prog::sym::ConstDeclTable* consts,
      std::vector<prog::sym::ConstId>* visibleConsts,
      prog::sym::TypeId typeHint) -> prog::expr::NodePtr;
};

} // namespace frontend::internal
