#include "lazy_table.hpp"
#include "utilities.hpp"

namespace frontend::internal {

auto LazyTable::getLazy(Context* ctx, prog::sym::TypeId result, bool isAction)
    -> prog::sym::TypeId {
  LazyMap& map = isAction ? m_lazyActions : m_lazies;

  // Try to find an existing lazy with the same result-type.
  auto itr = map.find(result);
  if (itr != map.end()) {
    return itr->second;
  }

  // Declare a new lazy.
  auto lazyName = std::string{isAction ? "__lazy_action_" : "__lazy_"} + getName(*ctx, result);
  const auto lazyType = ctx->getProg()->declareLazy(std::move(lazyName));

  // Define the lazy.
  ctx->getProg()->defineLazy(lazyType, result, isAction);

  // Keep track of some extra information about the type.
  ctx->declareTypeInfo(
      lazyType,
      TypeInfo{ctx, isAction ? "lazy_action" : "lazy", input::Span{0}, prog::sym::TypeSet{result}});

  map.insert({result, lazyType});
  return lazyType;
}

} // namespace frontend::internal
