#pragma once
#include "prog/sym/type_id.hpp"
#include <string>
#include <vector>

namespace prog::sym {

class UnionDef final {
  friend class TypeDefTable;

public:
  UnionDef()                        = delete;
  UnionDef(const UnionDef& rhs)     = default;
  UnionDef(UnionDef&& rhs) noexcept = default;
  ~UnionDef()                       = default;

  auto operator=(const UnionDef& rhs) -> UnionDef& = delete;
  auto operator=(UnionDef&& rhs) noexcept -> UnionDef& = delete;

  [[nodiscard]] auto getId() const noexcept -> const TypeId&;
  [[nodiscard]] auto getTypes() const noexcept -> const std::vector<sym::TypeId>&;
  [[nodiscard]] auto hasType(sym::TypeId type) const noexcept -> bool;
  [[nodiscard]] auto getTypeIndex(sym::TypeId type) const -> unsigned int;

private:
  sym::TypeId m_id;
  std::vector<sym::TypeId> m_types;

  UnionDef(sym::TypeId id, std::vector<sym::TypeId> types);
};

} // namespace prog::sym
