#include "prog/sym/type_def_table.hpp"
#include <stdexcept>

namespace prog::sym {

auto TypeDefTable::operator[](sym::TypeId id) const -> const TypeDef& {
  auto itr = m_types.find(id);
  if (itr == m_types.end()) {
    throw std::invalid_argument{"No definition has been registered for given type-id"};
  }
  return itr->second;
}

auto TypeDefTable::hasDef(sym::TypeId id) const -> bool {
  return m_types.find(id) != m_types.end();
}

auto TypeDefTable::begin() const -> Iterator { return m_types.begin(); }

auto TypeDefTable::end() const -> Iterator { return m_types.end(); }

auto TypeDefTable::registerStruct(
    const sym::TypeDeclTable& typeTable, sym::TypeId id, sym::FieldDeclTable fields) -> void {
  if (typeTable[id].getKind() != sym::TypeKind::Struct) {
    throw std::invalid_argument{"Type has not been declared as being a struct"};
  }
  auto itr = m_types.find(id);
  if (itr != m_types.end()) {
    throw std::logic_error{"Type already has a definition registered"};
  }
  m_types.insert({id, StructDef{id, std::move(fields)}});
}

auto TypeDefTable::registerUnion(
    const sym::TypeDeclTable& typeTable, sym::TypeId id, std::vector<sym::TypeId> types) -> void {
  if (typeTable[id].getKind() != sym::TypeKind::Union) {
    throw std::invalid_argument{"Type has not been declared as being a union"};
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

auto TypeDefTable::registerEnum(
    const sym::TypeDeclTable& typeTable,
    sym::TypeId id,
    std::unordered_map<std::string, int32_t> entries) -> void {
  if (typeTable[id].getKind() != sym::TypeKind::Enum) {
    throw std::invalid_argument{"Type has not been declared as being a enum"};
  }
  if (entries.empty()) {
    throw std::invalid_argument{"Enum needs at least one entry"};
  }
  auto itr = m_types.find(id);
  if (itr != m_types.end()) {
    throw std::logic_error{"Type already has a definition registered"};
  }
  m_types.insert({id, EnumDef{id, std::move(entries)}});
}

auto TypeDefTable::registerDelegate(
    const sym::TypeDeclTable& typeTable,
    sym::TypeId id,
    bool isAction,
    TypeSet input,
    TypeId output) -> void {

  if (typeTable[id].getKind() != sym::TypeKind::Delegate) {
    throw std::invalid_argument{"Type has not been declared as being a delegate"};
  }
  auto itr = m_types.find(id);
  if (itr != m_types.end()) {
    throw std::logic_error{"Type already has a definition registered"};
  }
  m_types.insert({id, DelegateDef{id, isAction, std::move(input), output}});
}

auto TypeDefTable::registerFuture(
    const sym::TypeDeclTable& typeTable, sym::TypeId id, TypeId result) -> void {

  if (typeTable[id].getKind() != sym::TypeKind::Future) {
    throw std::invalid_argument{"Type has not been declared as being a future"};
  }
  auto itr = m_types.find(id);
  if (itr != m_types.end()) {
    throw std::logic_error{"Type already has a definition registered"};
  }
  m_types.insert({id, FutureDef{id, result}});
}

} // namespace prog::sym
