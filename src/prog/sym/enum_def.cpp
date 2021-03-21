#include "prog/sym/enum_def.hpp"
#include <cassert>

namespace prog::sym {

EnumDef::EnumDef(sym::TypeId id, std::vector<Pair> entries) :
    m_id{id}, m_entries{std::move(entries)} {}

auto EnumDef::begin() const -> Iterator { return m_entries.begin(); }

auto EnumDef::end() const -> Iterator { return m_entries.end(); }

auto EnumDef::operator[](size_t idx) const -> const Pair& {
  assert(idx < m_entries.size());
  return m_entries[idx];
}

auto EnumDef::getId() const noexcept -> const TypeId& { return m_id; }

auto EnumDef::getCount() const noexcept -> size_t { return m_entries.size(); }

auto EnumDef::hasEntry(const std::string& name) const noexcept -> bool {
  for (const auto& entry : m_entries) {
    if (entry.first == name) {
      return true;
    }
  }
  return false;
}

auto EnumDef::getValue(const std::string& name) const noexcept -> std::optional<int32_t> {
  for (const auto& entry : m_entries) {
    if (entry.first == name) {
      return entry.second;
    }
  }
  return std::nullopt;
}

} // namespace prog::sym
