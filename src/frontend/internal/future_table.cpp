#include "future_table.hpp"
#include "utilities.hpp"

namespace frontend::internal {

auto FutureTable::getFuture(Context* ctx, prog::sym::TypeId result) -> prog::sym::TypeId {
  // Try to find an existing future with the same result-type.
  auto itr = m_futures.find(result);
  if (itr != m_futures.end()) {
    return itr->second;
  }

  // Declare a new future.
  auto futureName       = std::string{"__future_"} + getName(*ctx, result);
  const auto futureType = ctx->getProg()->declareFuture(std::move(futureName));

  // Define the future.
  ctx->getProg()->defineFuture(futureType, result);

  // Keep track of some extra information about the type.
  ctx->declareTypeInfo(
      futureType, TypeInfo{ctx, "future", input::Span{0}, prog::sym::TypeSet{result}});

  m_futures.insert({result, futureType});
  return futureType;
}

} // namespace frontend::internal
