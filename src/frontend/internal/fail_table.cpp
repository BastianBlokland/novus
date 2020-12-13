#include "fail_table.hpp"
#include "utilities.hpp"

namespace frontend::internal {

auto FailTable::getActionFail(Context* ctx, prog::sym::TypeId result) -> prog::sym::FuncId {
  // Try to find an existing fail action with the same result-type.
  auto itr = m_fails.find(result);
  if (itr != m_fails.end()) {
    return itr->second;
  }

  // Declare a new fail action.
  auto name       = std::string{"__failfast_"} + getName(*ctx, result);
  auto failAction = ctx->getProg()->declareActionFail(std::move(name), result);

  m_fails.insert({result, failAction});
  return failAction;
}

} // namespace frontend::internal
