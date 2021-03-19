#pragma once
#include "input/span.hpp"
#include "prog/sym/type_id.hpp"
#include <unordered_map>

namespace frontend::internal {

class Context;

class StaticIntTable final {
public:
  StaticIntTable()                              = default;
  StaticIntTable(const StaticIntTable& rhs)     = delete;
  StaticIntTable(StaticIntTable&& rhs) noexcept = default;
  ~StaticIntTable()                             = default;

  auto operator=(const StaticIntTable& rhs) -> StaticIntTable& = delete;
  auto operator=(StaticIntTable&& rhs) noexcept -> StaticIntTable& = delete;

  [[nodiscard]] auto getType(Context* ctx, int32_t val) -> prog::sym::TypeId;

private:
  std::unordered_map<int32_t, prog::sym::TypeId> m_types;
};

} // namespace frontend::internal
