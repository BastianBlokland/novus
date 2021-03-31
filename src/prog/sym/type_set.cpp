#include "prog/sym/type_set.hpp"
#include <cassert>

namespace prog::sym {

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
