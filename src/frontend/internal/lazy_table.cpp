#include "lazy_table.hpp"
#include "utilities.hpp"

namespace frontend::internal {

auto LazyTable::getLazy(Context* ctx, prog::sym::TypeId result) -> prog::sym::TypeId {
  // Try to find an existing lazy with the same result-type.
  auto itr = m_lazies.find(result);
  if (itr != m_lazies.end()) {
    return itr->second;
  }

  // Declare a new lazy.
  auto lazyName       = std::string{"__lazy_"} + getName(*ctx, result);
  const auto lazyType = ctx->getProg()->declareLazy(std::move(lazyName));

  // Define the lazy.
  ctx->getProg()->defineLazy(lazyType, result);

  // Keep track of some extra information about the type.
  ctx->declareTypeInfo(lazyType, TypeInfo{ctx, "lazy", input::Span{0}, prog::sym::TypeSet{result}});

  m_lazies.insert({result, lazyType});
  return lazyType;
}

} // namespace frontend::internal
