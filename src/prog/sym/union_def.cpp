#include "prog/sym/union_def.hpp"

namespace prog::sym {

UnionDef::UnionDef(sym::TypeId id, std::vector<sym::TypeId> types) :
    m_id{id}, m_types{std::move(types)} {}

auto UnionDef::getId() const noexcept -> const TypeId& { return m_id; }

auto UnionDef::getTypes() const noexcept -> const std::vector<sym::TypeId>& { return m_types; }

auto UnionDef::hasType(sym::TypeId type) const noexcept -> bool {
  return std::find(m_types.begin(), m_types.end(), type) != m_types.end();
}

} // namespace prog::sym
