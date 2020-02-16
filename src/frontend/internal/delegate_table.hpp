#pragma once
#include "prog/program.hpp"
#include <unordered_map>
#include <utility>

namespace frontend::internal {

class Context;

class DelegateTable final {
  using signature = typename std::pair<prog::sym::TypeSet, prog::sym::TypeId>;

  class Hasher final {
  public:
    auto operator()(const signature& id) const -> std::size_t;
  };

  using delegateSet = std::unordered_map<signature, prog::sym::TypeId, Hasher>;

public:
  DelegateTable()                             = default;
  DelegateTable(const DelegateTable& rhs)     = delete;
  DelegateTable(DelegateTable&& rhs) noexcept = default;
  ~DelegateTable()                            = default;

  auto operator=(const DelegateTable& rhs) -> DelegateTable& = delete;
  auto operator=(DelegateTable&& rhs) noexcept -> DelegateTable& = delete;

  auto getFunction(Context* ctx, const prog::sym::TypeSet& types) -> prog::sym::TypeId;

  auto getAction(Context* ctx, const prog::sym::TypeSet& types) -> prog::sym::TypeId;

  auto getFunction(Context* ctx, const prog::sym::TypeSet& input, prog::sym::TypeId output)
      -> prog::sym::TypeId;

  auto getAction(Context* ctx, const prog::sym::TypeSet& input, prog::sym::TypeId output)
      -> prog::sym::TypeId;

private:
  delegateSet m_functions;
  delegateSet m_actions;

  static auto
  getDelegate(Context* ctx, delegateSet* set, bool isAction, const prog::sym::TypeSet& types)
      -> prog::sym::TypeId;

  static auto getDelegate(
      Context* ctx,
      delegateSet* set,
      bool isAction,
      const prog::sym::TypeSet& input,
      prog::sym::TypeId output) -> prog::sym::TypeId;
};

} // namespace frontend::internal
