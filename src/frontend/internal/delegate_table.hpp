#pragma once
#include "prog/program.hpp"
#include <unordered_map>
#include <utility>

namespace frontend::internal {

class Context;

class DelegateTable final {
  using Signature = typename std::pair<prog::sym::TypeSet, prog::sym::TypeId>;

  class Hasher final {
  public:
    auto operator()(const Signature& id) const -> std::size_t;
  };

  // For each delegate we keep both a function and an action version.
  struct DelegateInfo {
    prog::sym::TypeId function;
    prog::sym::TypeId action;
  };

public:
  DelegateTable()                             = default;
  DelegateTable(const DelegateTable& rhs)     = delete;
  DelegateTable(DelegateTable&& rhs) noexcept = default;
  ~DelegateTable()                            = default;

  auto operator=(const DelegateTable& rhs) -> DelegateTable& = delete;
  auto operator=(DelegateTable&& rhs) noexcept -> DelegateTable& = delete;

  auto getDelegate(Context* ctx, bool isAction, const prog::sym::TypeSet& types)
      -> prog::sym::TypeId;

  auto getDelegate(
      Context* ctx, bool isAction, const prog::sym::TypeSet& input, prog::sym::TypeId output)
      -> prog::sym::TypeId;

private:
  std::unordered_map<Signature, DelegateInfo, Hasher> m_delegates;
};

} // namespace frontend::internal
