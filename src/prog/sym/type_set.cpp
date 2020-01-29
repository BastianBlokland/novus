#include "prog/sym/type_set.hpp"
#include <cassert>

namespace prog::sym {

TypeSet::TypeSet(std::initializer_list<TypeId> list) : m_val{list} {}

TypeSet::TypeSet(std::vector<TypeId> val) : m_val{std::move(val)} {}

auto TypeSet::operator[](unsigned int i) const -> TypeId {
  assert(i < this->m_val.size());
  return m_val[i];
}

auto TypeSet::operator==(const TypeSet& rhs) const noexcept -> bool { return m_val == rhs.m_val; }

auto TypeSet::operator!=(const TypeSet& rhs) const noexcept -> bool {
  return !TypeSet::operator==(rhs);
}

auto TypeSet::getCount() const -> unsigned int { return m_val.size(); }

auto TypeSet::begin() const -> iterator { return m_val.begin(); }

auto TypeSet::end() const -> iterator { return m_val.end(); }

auto TypeSet::withExtraType(TypeId type) -> TypeSet {
  auto types = m_val;
  types.push_back(type);
  return TypeSet{std::move(types)};
}

auto operator<<(std::ostream& out, const TypeSet& rhs) -> std::ostream& {
  for (auto i = 0U; i < rhs.m_val.size(); ++i) {
    if (i != 0) {
      out << ", ";
    }
    out << rhs.m_val[i];
  }
  return out;
}

} // namespace prog::sym
