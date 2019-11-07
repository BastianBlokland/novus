#include "prog/sym/const_decl_table.hpp"
#include <stdexcept>

namespace prog::sym {

auto ConstDeclTable::operator[](ConstId id) const -> const ConstDecl& {
  const auto index = id.m_id;
  assert(index < this->m_types.size());
  return m_types[index];
}

auto ConstDeclTable::begin() const -> iterator { return m_types.begin(); }

auto ConstDeclTable::end() const -> iterator { return m_types.end(); }

auto ConstDeclTable::getAll() const -> std::vector<ConstId> {
  auto result = std::vector<ConstId>{};
  result.reserve(m_types.size());
  for (const auto& c : m_types) {
    result.push_back(c.m_id);
  }
  return result;
}

auto ConstDeclTable::lookup(const std::string& name) const -> std::optional<ConstId> {
  const auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    return std::nullopt;
  }
  return itr->second;
}

auto ConstDeclTable::registerInput(std::string name, TypeId type) -> ConstId {
  return registerConst(ConstKind::Input, std::move(name), type);
}

auto ConstDeclTable::registerLocal(std::string name, TypeId type) -> ConstId {
  return registerConst(ConstKind::Local, std::move(name), type);
}

auto ConstDeclTable::registerConst(ConstKind kind, std::string name, TypeId type) -> ConstId {
  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }

  auto id = ConstId{static_cast<unsigned int>(m_types.size())};
  if (m_lookup.insert({name, id}).second) {
    m_types.push_back(ConstDecl{id, kind, std::move(name), type});
    return id;
  }
  throw std::logic_error{"Const with an identical name has already been registered"};
}

} // namespace prog::sym
