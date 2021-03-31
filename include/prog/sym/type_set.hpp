#pragma once
#include "prog/sym/type_id.hpp"
#include <cassert>
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
  TypeSet(std::initializer_list<TypeId> list) : m_val{list} {}
  explicit TypeSet(std::vector<TypeId> val) : m_val{std::move(val)} {}

  [[nodiscard]] auto operator[](unsigned int i) const -> TypeId {
    assert(i < this->m_val.size());
    return m_val[i];
  }

  auto operator==(const TypeSet& rhs) const noexcept -> bool { return m_val == rhs.m_val; }
  auto operator!=(const TypeSet& rhs) const noexcept -> bool { return !TypeSet::operator==(rhs); }

  [[nodiscard]] auto getCount() const -> unsigned int { return m_val.size(); }

  [[nodiscard]] auto begin() const -> Iterator { return m_val.begin(); }
  [[nodiscard]] auto end() const -> Iterator { return m_val.end(); }

  [[nodiscard]] auto withExtraType(TypeId type) -> TypeSet;

private:
  std::vector<TypeId> m_val;
};

auto operator<<(std::ostream& out, const TypeSet& rhs) -> std::ostream&;

} // namespace prog::sym
