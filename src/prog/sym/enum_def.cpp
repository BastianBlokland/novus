#include "prog/sym/enum_def.hpp"

namespace prog::sym {

EnumDef::EnumDef(sym::TypeId id, std::unordered_map<std::string, int32_t> entries) :
    m_id{id}, m_entries{std::move(entries)} {}

auto EnumDef::begin() const -> Iterator { return m_entries.begin(); }

auto EnumDef::end() const -> Iterator { return m_entries.end(); }

auto EnumDef::getId() const noexcept -> const TypeId& { return m_id; }

auto EnumDef::hasEntry(const std::string& name) const noexcept -> bool {
  return m_entries.find(name) != m_entries.end();
}

auto EnumDef::getValue(const std::string& name) const noexcept -> std::optional<int32_t> {
  const auto itr = m_entries.find(name);
  return itr == m_entries.end() ? std::nullopt : std::optional{itr->second};
}

} // namespace prog::sym
