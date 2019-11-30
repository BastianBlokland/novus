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

auto TypeDefTable::begin() const -> iterator { return m_types.begin(); }

auto TypeDefTable::end() const -> iterator { return m_types.end(); }

auto TypeDefTable::registerStruct(
    const sym::TypeDeclTable& typeTable, sym::TypeId id, std::vector<StructDef::Field> fields)
    -> void {
  if (typeTable[id].getKind() != sym::TypeKind::UserStruct) {
    throw std::invalid_argument{"Type has not been declared as being a user-defined struct"};
  }
  if (fields.empty()) {
    throw std::invalid_argument{"Struct needs atleast one field"};
  }
  auto itr = m_types.find(id);
  if (itr != m_types.end()) {
    throw std::logic_error{"Type already has a definition registered"};
  }
  m_types.insert({id, StructDef{id, std::move(fields)}});
}

} // namespace prog::sym
