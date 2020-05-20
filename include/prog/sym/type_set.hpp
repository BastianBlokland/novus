#pragma once
#include "prog/sym/type_id.hpp"
#include <initializer_list>
#include <iostream>
#include <vector>

namespace prog::sym {

// Immutable collection of types.
class TypeSet final {
  friend auto operator<<(std::ostream& out, const TypeSet& rhs) -> std::ostream&;

public:
  using Iterator = typename std::vector<TypeId>::const_iterator;

  TypeSet() = default;
  TypeSet(std::initializer_list<TypeId> list);
  explicit TypeSet(std::vector<TypeId> val);

  [[nodiscard]] auto operator[](unsigned int) const -> TypeId;

  auto operator==(const TypeSet& rhs) const noexcept -> bool;
  auto operator!=(const TypeSet& rhs) const noexcept -> bool;

  [[nodiscard]] auto getCount() const -> unsigned int;

  [[nodiscard]] auto begin() const -> Iterator;
  [[nodiscard]] auto end() const -> Iterator;

  [[nodiscard]] auto withExtraType(TypeId type) -> TypeSet;

private:
  std::vector<TypeId> m_val;
};

auto operator<<(std::ostream& out, const TypeSet& rhs) -> std::ostream&;

} // namespace prog::sym
