#pragma once
#include "prog/sym/type_id.hpp"

namespace prog::sym {

class LazyDef final {
  friend class TypeDefTable;

public:
  LazyDef()                       = delete;
  LazyDef(const LazyDef& rhs)     = default;
  LazyDef(LazyDef&& rhs) noexcept = default;
  ~LazyDef()                      = default;

  auto operator=(const LazyDef& rhs) -> LazyDef& = delete;
  auto operator=(LazyDef&& rhs) noexcept -> LazyDef& = delete;

  [[nodiscard]] auto getId() const noexcept -> const TypeId&;
  [[nodiscard]] auto getResult() const -> const TypeId&;

private:
  sym::TypeId m_id;
  TypeId m_result;

  LazyDef(sym::TypeId id, TypeId result);
};

} // namespace prog::sym
