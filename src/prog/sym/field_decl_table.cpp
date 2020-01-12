#include "prog/sym/field_decl_table.hpp"
#include <cassert>
#include <stdexcept>

namespace prog::sym {

auto FieldDeclTable::operator[](FieldId id) const -> const FieldDecl& {
  const auto index = id.m_id;
  assert(index < this->m_fields.size());
  return m_fields[index];
}

auto FieldDeclTable::getCount() const -> unsigned int { return m_fields.size(); }

auto FieldDeclTable::begin() const -> iterator { return m_fields.begin(); }

auto FieldDeclTable::end() const -> iterator { return m_fields.end(); }

auto FieldDeclTable::lookup(const std::string& name) const -> std::optional<FieldId> {
  const auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    return std::nullopt;
  }
  return itr->second;
}

auto FieldDeclTable::registerField(std::string name, TypeId type) -> FieldId {
  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }

  auto id = FieldId{static_cast<unsigned int>(m_fields.size())};
  if (m_lookup.insert({name, id}).second) {
    m_fields.push_back(FieldDecl{id, std::move(name), type});
    return id;
  }
  throw std::logic_error{"Field with an identical name has already been registered"};
}

} // namespace prog::sym
