#include "prog/sym/action_decl_table.hpp"
#include <stdexcept>

namespace prog::sym {

auto ActionDeclTable::operator[](ActionId id) const -> const ActionDecl& {
  const auto index = id.m_id;
  assert(index < this->m_actions.size());
  return m_actions[index];
}

auto ActionDeclTable::begin() const -> iterator { return m_actions.begin(); }

auto ActionDeclTable::end() const -> iterator { return m_actions.end(); }

auto ActionDeclTable::lookup(const std::string& name) const -> std::vector<ActionId> {
  const auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    return std::vector<ActionId>{};
  }
  return itr->second;
}

auto ActionDeclTable::lookup(const std::string& name, const Input& input) const
    -> std::optional<ActionId> {
  const auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    return std::nullopt;
  }
  return findOverload(itr->second, input);
}

auto ActionDeclTable::registerAction(ActionKind kind, std::string name, Input input) -> ActionId {
  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }

  auto id  = ActionId{static_cast<unsigned int>(m_actions.size())};
  auto itr = m_lookup.find(name);
  if (itr == m_lookup.end()) {
    itr = m_lookup.insert({name, std::vector<ActionId>{}}).first;
  }
  if (findOverload(itr->second, input)) {
    throw std::logic_error{"Action with an identical name and input has already been registered"};
  }
  itr->second.push_back(id);
  m_actions.push_back(ActionDecl{id, kind, std::move(name), std::move(input)});
  return id;
}

auto ActionDeclTable::findOverload(const std::vector<ActionId>& overloads, const Input& input) const
    -> std::optional<ActionId> {
  for (const auto& actionId : overloads) {
    const auto& actionDecl = ActionDeclTable::operator[](actionId);
    if (actionDecl.m_input == input) {
      return actionId;
    }
  }
  return std::nullopt;
}

} // namespace prog::sym
