#include "prog/sym/type_decl_table.hpp"
#include <stdexcept>

namespace prog::sym {

auto TypeDeclTable::operator[](TypeId id) const -> const TypeDecl& {
  const auto index = id.m_id;
  assert(index < this->m_types.size());
  return m_types[index];
}

auto TypeDeclTable::begin() const -> iterator { return m_types.begin(); }

auto TypeDeclTable::end() const -> iterator { return m_types.end(); }

auto TypeDeclTable::lookup(const std::string& name) const -> std::optional<TypeId> {
  const auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    return std::nullopt;
  }
  return itr->second;
}

auto TypeDeclTable::registerIntrinsic(std::string name) -> TypeId {
  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }

  auto id = TypeId{static_cast<unsigned int>(m_types.size())};
  if (m_lookup.insert({name, id}).second) {
    m_types.push_back(TypeDecl{id, TypeKind::Intrinsic, std::move(name)});
    return id;
  }
  throw std::logic_error{"Type with an identical name has already been registered"};
}

} // namespace prog::sym
