#include "fail_table.hpp"
#include "utilities.hpp"

namespace frontend::internal {

auto FailTable::getFailIntrinsic(Context* ctx, prog::sym::TypeId result) -> prog::sym::FuncId {
  // Try to find an existing fail intrinsic with the same result-type.
  auto itr = m_fails.find(result);
  if (itr != m_fails.end()) {
    return itr->second;
  }

  // Declare a new fail intrinsic.
  auto name          = std::string{"__fail_"} + getName(*ctx, result);
  auto failIntrinsic = ctx->getProg()->declareFailIntrinsic(std::move(name), result);

  m_fails.insert({result, failIntrinsic});
  return failIntrinsic;
}

} // namespace frontend::internal
