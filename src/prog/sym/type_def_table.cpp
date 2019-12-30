#include "prog/sym/type_def_table.hpp"
#include <stdexcept>

namespace prog::sym {

auto TypeDefTable::operator[](sym::TypeId id) const -> const typeDef& {
  auto itr = m_types.find(id);
  if (itr == m_types.end()) {
    throw std::invalid_argument{"No definition has been registered for given type-id"};
  }
  return itr->second;
}

auto TypeDefTable::hasDef(sym::TypeId id) const -> bool {
  return m_types.find(id) != m_types.end();
}

auto TypeDefTable::begin() const -> iterator { return m_types.begin(); }

auto TypeDefTable::end() const -> iterator { return m_types.end(); }

auto TypeDefTable::registerStruct(
    const sym::TypeDeclTable& typeTable, sym::TypeId id, sym::FieldDeclTable fields) -> void {
  if (typeTable[id].getKind() != sym::TypeKind::UserStruct) {
    throw std::invalid_argument{"Type has not been declared as being a user-defined struct"};
  }
  auto itr = m_types.find(id);
  if (itr != m_types.end()) {
    throw std::logic_error{"Type already has a definition registered"};
  }
  m_types.insert({id, StructDef{id, std::move(fields)}});
}

auto TypeDefTable::registerUnion(
    const sym::TypeDeclTable& typeTable, sym::TypeId id, std::vector<sym::TypeId> types) -> void {
  if (typeTable[id].getKind() != sym::TypeKind::UserUnion) {
    throw std::invalid_argument{"Type has not been declared as being a user-defined union"};
  }
  if (types.size() < 2) {
    throw std::invalid_argument{"Union needs at least two types"};
  }
  auto itr = m_types.find(id);
  if (itr != m_types.end()) {
    throw std::logic_error{"Type already has a definition registered"};
  }
  m_types.insert({id, UnionDef{id, std::move(types)}});
}

auto TypeDefTable::registerDelegate(
    const sym::TypeDeclTable& typeTable, sym::TypeId id, TypeSet input, TypeId output) -> void {
  if (typeTable[id].getKind() != sym::TypeKind::UserDelegate) {
    throw std::invalid_argument{"Type has not been declared as being a user-defined delegate"};
  }
  auto itr = m_types.find(id);
  if (itr != m_types.end()) {
    throw std::logic_error{"Type already has a definition registered"};
  }
  m_types.insert({id, DelegateDef{id, std::move(input), output}});
}

} // namespace prog::sym
