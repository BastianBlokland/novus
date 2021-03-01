#include "frontend/source_table.hpp"
#include "prog/sym/source_id.hpp"
#include <stdexcept>

namespace frontend {

SourceTable::SourceTable(Map map) : m_map{std::move(map)} {}

auto SourceTable::operator[](prog::sym::SourceId source) const -> SourceInfo {
  if (source == prog::sym::SourceId::none()) {
    throw std::logic_error{"The 'none' SourceId has no information associated with it"};
  }
  const auto itr = m_map.find(source);
  if (itr == m_map.end()) {
    throw std::logic_error{"SourceId is not present in this table"};
  }
  return itr->second;
}

} // namespace frontend
