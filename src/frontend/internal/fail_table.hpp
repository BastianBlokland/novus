#pragma once
#include "prog/program.hpp"
#include <unordered_map>

namespace frontend::internal {

class Context;

class FailTable final {
public:
  FailTable()                         = default;
  FailTable(const FailTable& rhs)     = delete;
  FailTable(FailTable&& rhs) noexcept = default;
  ~FailTable()                        = default;

  auto operator=(const FailTable& rhs) -> FailTable& = delete;
  auto operator=(FailTable&& rhs) noexcept -> FailTable& = delete;

  auto getFailIntrinsic(Context* ctx, prog::sym::TypeId result) -> prog::sym::FuncId;

private:
  std::unordered_map<prog::sym::TypeId, prog::sym::FuncId, prog::sym::TypeIdHasher> m_fails;
};

} // namespace frontend::internal
