#pragma once
#include "prog/program.hpp"
#include <unordered_map>

namespace frontend::internal {

class Context;

class LazyTable final {
public:
  LazyTable()                         = default;
  LazyTable(const LazyTable& rhs)     = delete;
  LazyTable(LazyTable&& rhs) noexcept = default;
  ~LazyTable()                        = default;

  auto operator=(const LazyTable& rhs) -> LazyTable& = delete;
  auto operator=(LazyTable&& rhs) noexcept -> LazyTable& = delete;

  auto getLazy(Context* ctx, prog::sym::TypeId result, bool isAction) -> prog::sym::TypeId;

private:
  using LazyMap = std::unordered_map<prog::sym::TypeId, prog::sym::TypeId, prog::sym::TypeIdHasher>;

  LazyMap m_lazies;
  LazyMap m_lazyActions;
};

} // namespace frontend::internal
