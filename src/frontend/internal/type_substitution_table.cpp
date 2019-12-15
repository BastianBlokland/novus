#include "internal/type_substitution_table.hpp"

namespace frontend::internal {

[[nodiscard]] auto TypeSubstitutionTable::lookupType(const std::string& name) const
    -> std::optional<prog::sym::TypeId> {
  const auto itr = m_subs.find(name);
  if (itr == m_subs.end()) {
    return std::nullopt;
  }
  return itr->second;
}

auto TypeSubstitutionTable::declare(std::string name, prog::sym::TypeId type) -> void {
  if (name.empty()) {
    throw std::invalid_argument{"Name has to contain aleast 1 char"};
  }
  if (!m_subs.insert({std::move(name), type}).second) {
    throw std::logic_error{"Substitution with an identical name has already been registered"};
  }
}

} // namespace frontend::internal
