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

auto DelegateTable::getDelegate(Context* ctx, bool isAction, const prog::sym::TypeSet& types)
    -> prog::sym::TypeId {

  auto inputTypes = std::vector<prog::sym::TypeId>{};
  for (auto i = 0U; i != types.getCount() - 1; ++i) {
    inputTypes.push_back(types[i]);
  }
  auto outputType = types[types.getCount() - 1];
  return getDelegate(ctx, isAction, prog::sym::TypeSet{std::move(inputTypes)}, outputType);
}

auto DelegateTable::getDelegate(
    Context* ctx, bool isAction, const prog::sym::TypeSet& input, prog::sym::TypeId output)
    -> prog::sym::TypeId {

  /* This always makes function and action (pure and impure) versions with the same signature,
  reason is that we can support making the function (pure) version implicitly convertible to the
  action (impure) version. */

  auto sig = std::make_pair(input, output);

  // Try to find an existing delegate with the same signature.
  auto itr = m_delegates.find(sig);
  if (itr != m_delegates.end()) {
    // We store the function version as the first of the pair and the action as the second.
    return isAction ? itr->second.second : itr->second.first;
  }

  // Declare the delegates (both function and action versions).
  const auto funcDeclType =
      ctx->getProg()->declareDelegate(getDelegateName(*ctx, false, input, output));
  const auto actionDeclType =
      ctx->getProg()->declareDelegate(getDelegateName(*ctx, true, input, output));

  // Define the action delegate.
  ctx->getProg()->defineDelegate(actionDeclType, true, input, output, {});

  // Define the function delegate, give the action version as a alias so that function delegates are
  // implicitly convertible to action delegates.
  ctx->getProg()->defineDelegate(funcDeclType, false, input, output, {actionDeclType});

  // Keep track of some extra information about the types.
  auto types = std::vector<prog::sym::TypeId>{};
  types.insert(types.end(), input.begin(), input.end());
  types.push_back(output);

  ctx->declareTypeInfo(
      funcDeclType, TypeInfo{ctx, "function", input::Span{0}, prog::sym::TypeSet{types}});
  ctx->declareTypeInfo(
      actionDeclType, TypeInfo{ctx, "action", input::Span{0}, prog::sym::TypeSet{types}});

  m_delegates.insert({std::move(sig), std::make_pair(funcDeclType, actionDeclType)});
  return isAction ? actionDeclType : funcDeclType;
}

} // namespace frontend::internal
