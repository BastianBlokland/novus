#include "internal/const_binder.hpp"
#include <algorithm>
#include <sstream>

namespace frontend::internal {

ConstBinder::ConstBinder(
    prog::sym::ConstDeclTable* consts,
    std::vector<prog::sym::ConstId>* visibleConsts,
    ConstBinder* parent) :
    m_consts{consts}, m_visibleConsts{visibleConsts}, m_parent{parent} {

  if (m_consts == nullptr) {
    throw std::invalid_argument{"Constants table cannot be null"};
  }
  if (m_visibleConsts == nullptr) {
    throw std::invalid_argument{"Visible constants vector cannot be null"};
  }
}

auto ConstBinder::getConsts() -> prog::sym::ConstDeclTable* { return m_consts; }

auto ConstBinder::getVisibleConsts() -> std::vector<prog::sym::ConstId>* { return m_visibleConsts; }

auto ConstBinder::setVisibleConsts(std::vector<prog::sym::ConstId>* visibleConsts) -> void {
  m_visibleConsts = visibleConsts;
}

auto ConstBinder::getParent() -> ConstBinder* { return m_parent; }

auto ConstBinder::getBoundParentConsts() const noexcept -> const std::vector<prog::sym::ConstId>& {
  return m_boundParentConsts;
}

auto ConstBinder::registerLocal(std::string name, prog::sym::TypeId type) -> prog::sym::ConstId {
  const auto constId = m_consts->registerLocal(std::move(name), type);
  m_visibleConsts->push_back(constId);
  return constId;
}

auto ConstBinder::canBind(const std::string& name) const -> bool {
  const auto constId = m_consts->lookup(name);
  if (constId &&
      std::find(m_visibleConsts->begin(), m_visibleConsts->end(), *constId) !=
          m_visibleConsts->end()) {
    return true;
  }
  return m_parent != nullptr && m_parent->canBind(name);
}

auto ConstBinder::doesExistButNotVisible(const std::string& name) const -> bool {
  const auto constId = m_consts->lookup(name);
  if (constId &&
      std::find(m_visibleConsts->begin(), m_visibleConsts->end(), *constId) ==
          m_visibleConsts->end()) {
    return true;
  }
  return m_parent != nullptr && m_parent->doesExistButNotVisible(name);
}

auto ConstBinder::bind(const std::string& name) -> std::optional<prog::sym::ConstId> {
  const auto localConst = m_consts->lookup(name);
  if (localConst &&
      std::find(m_visibleConsts->begin(), m_visibleConsts->end(), *localConst) !=
          m_visibleConsts->end()) {
    return localConst;
  }

  if (!m_parent) {
    return std::nullopt;
  }

  // Find a parent constant we bound before.
  const auto itr = m_boundParentConstsLookup.find(name);
  if (itr != m_boundParentConstsLookup.end()) {
    return itr->second;
  }

  // Find a parent const to bind to.
  const auto parentConst = m_parent->bind(name);
  if (!parentConst) {
    return std::nullopt;
  }
  const auto parentConstType = (*m_parent->m_consts)[*parentConst].getType();

  // Declare the const as a bound const in the local table.
  const auto boundLocalConst = m_consts->registerBound(parentConstType);

  // Add it to the bound lookup.
  m_boundParentConsts.push_back(*parentConst);
  m_boundParentConstsLookup.insert({name, boundLocalConst});

  return boundLocalConst;
}

auto ConstBinder::getAllConstTypes() -> std::unordered_map<std::string, prog::sym::TypeId> {
  auto result = std::unordered_map<std::string, prog::sym::TypeId>{};

  ConstBinder* current = this;
  while (current) {
    for (const auto& visibleConst : *current->m_visibleConsts) {
      const auto& name = (*current->m_consts)[visibleConst].getName();
      const auto& type = (*current->m_consts)[visibleConst].getType();
      result.insert({name, type});
    }
    current = current->m_parent;
  }

  return result;
}

} // namespace frontend::internal
