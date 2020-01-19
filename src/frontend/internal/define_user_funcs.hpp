#pragma once
#include "internal/context.hpp"
#include "internal/type_substitution_table.hpp"

namespace frontend::internal {

class DefineUserFuncs final {
public:
  DefineUserFuncs() = delete;
  DefineUserFuncs(Context* context, const TypeSubstitutionTable* typeSubTable);

  template <typename FuncParseNode>
  auto define(prog::sym::FuncId id, std::string funcDisplayName, const FuncParseNode& n) -> bool;

private:
  Context* m_context;
  const TypeSubstitutionTable* m_typeSubTable;

  auto getExpr(
      const parse::Node& n,
      prog::sym::ConstDeclTable* consts,
      std::vector<prog::sym::ConstId>* visibleConsts,
      prog::sym::TypeId typeHint,
      bool allowActionCalls) -> prog::expr::NodePtr;
};

} // namespace frontend::internal
