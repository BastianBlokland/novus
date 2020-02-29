#include "prog/sym/const_decl_table.hpp"
#include <algorithm>
#include <cassert>
#include <sstream>
#include <stdexcept>

namespace prog::sym {

auto ConstDeclTable::operator[](ConstId id) const -> const ConstDecl& {
  const auto index = getOffset(id);
  assert(index < this->m_consts.size());
  return m_consts[index];
}

auto ConstDeclTable::getCount() const -> unsigned int { return m_consts.size(); }

auto ConstDeclTable::getLocalCount() const -> unsigned int {
  return std::count_if(
      m_consts.begin(), m_consts.end(), [](const auto& c) { return c.m_kind == ConstKind::Local; });
}

auto ConstDeclTable::begin() const -> Iterator { return m_consts.begin(); }

auto ConstDeclTable::begin(const ConstKind kind) const -> Iterator {
  return std::lower_bound(
      m_consts.begin(), m_consts.end(), kind, [](const ConstDecl& a, const ConstKind& b) {
        return a.m_kind < b;
      });
}

auto ConstDeclTable::end() const -> Iterator { return m_consts.end(); }

auto ConstDeclTable::end(const ConstKind kind) const -> Iterator {
  return std::upper_bound(
      m_consts.begin(), m_consts.end(), kind, [](const ConstKind& a, const ConstDecl& b) {
        return a < b.m_kind;
      });
}

auto ConstDeclTable::getAll() const -> std::vector<ConstId> {
  return getConstIds(ConstKind::Input, ConstKind::Local);
}

auto ConstDeclTable::getInputs() const -> std::vector<ConstId> {
  return getConstIds(ConstKind::Input, ConstKind::Bound);
}

auto ConstDeclTable::getBoundInputs() const -> std::vector<ConstId> {
  return getConstIds(ConstKind::Bound, ConstKind::Bound);
}

auto ConstDeclTable::getNonBoundInputs() const -> std::vector<ConstId> {
  return getConstIds(ConstKind::Input, ConstKind::Input);
}

auto ConstDeclTable::lookup(const std::string& name) const -> std::optional<ConstId> {
  const auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    return std::nullopt;
  }
  return itr->second;
}

auto ConstDeclTable::getOffset(ConstId id) const -> unsigned int {
  return std::find_if(begin(), end(), [id](const ConstDecl& decl) { return decl.m_id == id; }) -
      begin();
}

auto ConstDeclTable::registerBound(TypeId type) -> ConstId {
  std::ostringstream oss;
  oss << "__bound_" << m_consts.size();
  return registerConst(ConstKind::Bound, oss.str(), type);
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

  auto id = ConstId{static_cast<unsigned int>(m_consts.size())};
  if (m_lookup.insert({name, id}).second) {
    // Keep entries in m_consts sorted by kind.
    m_consts.insert(end(kind), ConstDecl{id, kind, std::move(name), type});
    return id;
  }
  throw std::logic_error{"Const with an identical name has already been registered"};
}

auto ConstDeclTable::getConstIds(ConstKind beginKind, ConstKind endKind) const
    -> std::vector<ConstId> {

  auto beginItr = begin(beginKind);
  auto endItr   = end(endKind);

  auto result = std::vector<ConstId>{};
  result.reserve(endItr - beginItr);
  std::transform(
      beginItr, endItr, std::back_inserter(result), [](const ConstDecl& c) { return c.m_id; });

  return result;
}

} // namespace prog::sym
