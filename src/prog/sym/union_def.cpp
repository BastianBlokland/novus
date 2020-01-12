#include "prog/sym/union_def.hpp"
#include <algorithm>

namespace prog::sym {

UnionDef::UnionDef(sym::TypeId id, std::vector<sym::TypeId> types) :
    m_id{id}, m_types{std::move(types)} {}

auto UnionDef::getId() const noexcept -> const TypeId& { return m_id; }

auto UnionDef::getTypes() const noexcept -> const std::vector<sym::TypeId>& { return m_types; }

auto UnionDef::hasType(sym::TypeId type) const noexcept -> bool {
  return std::find(m_types.begin(), m_types.end(), type) != m_types.end();
}

auto UnionDef::getTypeIndex(sym::TypeId type) const -> unsigned int {
  const auto itr = std::find(m_types.begin(), m_types.end(), type);
  if (itr == m_types.end()) {
    throw std::invalid_argument{"Given type is not part of the union"};
  }
  return itr - m_types.begin();
}

} // namespace prog::sym
