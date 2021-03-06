#include "internal/lazy_table.hpp"
#include "internal/utilities.hpp"

namespace frontend::internal {

auto LazyTable::getLazy(Context* ctx, prog::sym::TypeId result, bool isAction)
    -> prog::sym::TypeId {

  /* This always makes a pure and an impure version of the lazy type, the reason fis that we can
   * support making the pure version implicitly convertible to the impure version. */

  // Try to find an existing lazy with the same result-type.
  auto itr = m_map.find(result);
  if (itr != m_map.end()) {
    return isAction ? itr->second.lazyAction : itr->second.lazy;
  }

  // Declare a new lazy (both a pure and an impure version).
  const auto lazyType = ctx->getProg()->declareLazy(std::string{"__lazy_"} + getName(*ctx, result));
  const auto lazyActionType =
      ctx->getProg()->declareLazy(std::string{"__lazy_action_"} + getName(*ctx, result));

  // Define the impure lazy.
  ctx->getProg()->defineLazy(lazyActionType, result, true, {});

  // Define the pure lazy, give the action version as a alias so that 'lazy' is implicitly
  // convertible to 'lazy_action'.
  ctx->getProg()->defineLazy(lazyType, result, false, {lazyActionType});

  // Keep track of some extra information about the types.
  ctx->declareTypeInfo(lazyType, TypeInfo{ctx, "lazy", input::Span{0}, prog::sym::TypeSet{result}});
  ctx->declareTypeInfo(
      lazyActionType, TypeInfo{ctx, "lazy_action", input::Span{0}, prog::sym::TypeSet{result}});

  m_map.insert({result, LazyInfo{lazyType, lazyActionType}});
  return isAction ? lazyActionType : lazyType;
}

} // namespace frontend::internal
