#pragma once
#include "prog/program.hpp"
#include <unordered_map>

namespace frontend::internal {

class Context;

class FutureTable final {
public:
  FutureTable()                           = default;
  FutureTable(const FutureTable& rhs)     = delete;
  FutureTable(FutureTable&& rhs) noexcept = default;
  ~FutureTable()                          = default;

  auto operator=(const FutureTable& rhs) -> FutureTable& = delete;
  auto operator=(FutureTable&& rhs) noexcept -> FutureTable& = delete;

  auto getFuture(Context* ctx, prog::sym::TypeId result) -> prog::sym::TypeId;

private:
  std::unordered_map<prog::sym::TypeId, prog::sym::TypeId, prog::sym::TypeIdHasher> m_futures;
};

} // namespace frontend::internal
