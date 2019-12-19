#include "internal/typeinfer_user_funcs.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/typeinfer_expr.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

TypeInferUserFuncs::TypeInferUserFuncs(
    Context* context, const TypeSubstitutionTable* typeSubTable) :
    m_context{context}, m_typeSubTable{typeSubTable} {

  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto TypeInferUserFuncs::inferRetType(prog::sym::FuncId id, const parse::FuncDeclStmtNode& n)
    -> bool {

  // If we've processed this func before (and failed) we try more aggressively.
  // Reason for this is that in the first iteration we only make a decision if we are sure that its
  // the best type (mostly related to conversions), and hope that more information will become
  // available when we type-infer the other functions. In the second 'aggressive' iteration we trust
  // that this is all the information we will get and make a choice based on that.
  auto agressive = m_processed.find(id) != m_processed.end();
  m_processed.insert(id);

  auto constTypes = std::unordered_map<std::string, prog::sym::TypeId>{};
  for (const auto& arg : n.getArgs()) {
    const auto argType = m_context->getProg()->lookupType(getName(arg.getType()));
    if (argType) {
      constTypes.insert({getName(arg.getIdentifier()), *argType});
    }
  }

  auto inferBodyType = TypeInferExpr{m_context, m_typeSubTable, &constTypes, agressive};
  n[0].accept(&inferBodyType);
  const auto type = inferBodyType.getInferredType();

  // If type is still not a concrete type then we fail.
  if (!type.isConcrete()) {
    return false;
  }

  // Update function output with inferred type.
  m_context->getProg()->updateFuncOutput(id, type);
  return true;
}

} // namespace frontend::internal
