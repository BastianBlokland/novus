#include "delegate_table.hpp"
#include "utilities.hpp"

namespace frontend::internal {

static auto getDelegateName(
    const Context& ctx, bool isAction, const prog::sym::TypeSet& input, prog::sym::TypeId output)
    -> std::string {
  auto result = std::string{isAction ? "__action" : "__function"};
  for (const auto& type : input) {
    const auto& typeName = getName(ctx, type);
    result += '_' + typeName;
  }
  result += '_' + getName(ctx, output);
  return result;
}

auto DelegateTable::Hasher::operator()(const signature& id) const -> std::size_t {
  std::size_t result = id.first.getCount();
  for (auto& i : id.first) {
    result ^= i.getNum() + 0x9e3779b9 + (result << 6U) + (result >> 2U); // NOLINT: Magic numbers
  }
  result ^=
      id.second.getNum() + 0x9e3779b9 + (result << 6U) + (result >> 2U); // NOLINT: Magic numbers
  return result;
}

auto DelegateTable::getFunction(Context* ctx, const prog::sym::TypeSet& types)
    -> prog::sym::TypeId {
  return getDelegate(ctx, &m_functions, false, types);
}

auto DelegateTable::getAction(Context* ctx, const prog::sym::TypeSet& types) -> prog::sym::TypeId {
  return getDelegate(ctx, &m_actions, true, types);
}

auto DelegateTable::getFunction(
    Context* ctx, const prog::sym::TypeSet& input, prog::sym::TypeId output) -> prog::sym::TypeId {
  return getDelegate(ctx, &m_functions, false, input, output);
}

auto DelegateTable::getAction(
    Context* ctx, const prog::sym::TypeSet& input, prog::sym::TypeId output) -> prog::sym::TypeId {
  return getDelegate(ctx, &m_actions, true, input, output);
}

auto DelegateTable::getDelegate(
    Context* ctx, delegateSet* set, bool isAction, const prog::sym::TypeSet& types)
    -> prog::sym::TypeId {

  auto inputTypes = std::vector<prog::sym::TypeId>{};
  for (auto i = 0U; i != types.getCount() - 1; ++i) {
    inputTypes.push_back(types[i]);
  }
  auto outputType = types[types.getCount() - 1];
  return getDelegate(ctx, set, isAction, prog::sym::TypeSet{std::move(inputTypes)}, outputType);
}

auto DelegateTable::getDelegate(
    Context* ctx,
    delegateSet* set,
    bool isAction,
    const prog::sym::TypeSet& input,
    prog::sym::TypeId output) -> prog::sym::TypeId {

  auto sig = std::make_pair(input, output);

  // Try to find an existing delegate with the same signature.
  auto itr = set->find(sig);
  if (itr != set->end()) {
    return itr->second;
  }

  // Declare a new delegate.
  auto delegateName       = getDelegateName(*ctx, isAction, input, output);
  const auto delegateType = ctx->getProg()->declareDelegate(delegateName);

  // Define the delegate.
  ctx->getProg()->defineDelegate(delegateType, isAction, input, output);

  // Keep track of some extra information about the type.
  auto types = std::vector<prog::sym::TypeId>{};
  types.insert(types.end(), input.begin(), input.end());
  types.push_back(output);
  ctx->declareTypeInfo(
      delegateType,
      TypeInfo{ctx,
               isAction ? "action" : "function",
               input::Span{0},
               prog::sym::TypeSet{std::move(types)}});

  set->insert({std::move(sig), delegateType});
  return delegateType;
}

} // namespace frontend::internal
