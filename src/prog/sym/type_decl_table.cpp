#include "prog/sym/type_decl_table.hpp"
#include <cassert>
#include <stdexcept>

namespace prog::sym {

const unsigned int reservedTypesCount = 1;

auto TypeDeclTable::operator[](TypeId id) const -> const TypeDecl& {
  const auto itr = m_types.find(id);
  if (itr == m_types.end()) {
    throw std::invalid_argument{"No declaration found for given type-id"};
  }
  return itr->second;
}

auto TypeDeclTable::begin() const -> Iterator { return m_types.begin(); }

auto TypeDeclTable::end() const -> Iterator { return m_types.end(); }

auto TypeDeclTable::exists(const TypeId& id) const -> bool {
  return m_types.find(id) != m_types.end();
}

auto TypeDeclTable::exists(const std::string& name) const -> bool {
  return lookup(name).has_value();
}

auto TypeDeclTable::lookup(const std::string& name) const -> std::optional<TypeId> {
  const auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    return std::nullopt;
  }
  return itr->second;
}

auto TypeDeclTable::registerType(TypeKind kind, std::string name) -> TypeId {
  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }

  // Assign an id one higher then the current highest, starting from 'reservedTypesCount'.
  const auto highestKey = m_types.rbegin();
  const auto id =
      TypeId{highestKey == m_types.rend() ? reservedTypesCount : highestKey->first.m_id + 1};

  if (m_lookup.insert({name, id}).second) {
    m_types.insert({id, TypeDecl{id, kind, std::move(name)}});
    return id;
  }
  throw std::logic_error{"Type with an identical name has already been registered"};
}

auto TypeDeclTable::insertType(TypeId id, TypeKind kind, std::string name) -> void {
  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }

  // Insert into types map.
  if (!m_types.insert({id, TypeDecl{id, kind, name}}).second) {
    throw std::invalid_argument{"There is already a type registered with the same id"};
  }

  // Insert into name lookup map.
  m_lookup.insert({std::move(name), id});
}

} // namespace prog::sym
