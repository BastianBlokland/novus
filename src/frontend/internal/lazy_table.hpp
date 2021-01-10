#pragma once
#include "prog/program.hpp"
#include <unordered_map>

namespace frontend::internal {

class Context;

class LazyTable final {
  // For each lazy we keep a pure and an impure version.
  struct LazyInfo {
    prog::sym::TypeId lazy;
    prog::sym::TypeId lazyAction;
  };

public:
  LazyTable()                         = default;
  LazyTable(const LazyTable& rhs)     = delete;
  LazyTable(LazyTable&& rhs) noexcept = default;
  ~LazyTable()                        = default;

  auto operator=(const LazyTable& rhs) -> LazyTable& = delete;
  auto operator=(LazyTable&& rhs) noexcept -> LazyTable& = delete;

  auto getLazy(Context* ctx, prog::sym::TypeId result, bool isAction) -> prog::sym::TypeId;

private:
  using LazyMap = std::unordered_map<prog::sym::TypeId, LazyInfo, prog::sym::TypeIdHasher>;

  LazyMap m_map;
};

} // namespace frontend::internal
