#include "source_table_builder.hpp"
#include <cassert>
#include <stdexcept>

namespace frontend::internal {

SourceTableBuilder::SourceTableBuilder() noexcept : m_build{false}, m_srcIdNum{0} {}

auto SourceTableBuilder::add(const Source* src, input::Span span) -> prog::sym::SourceId {
  if (m_build) {
    throw std::logic_error{"SourceTable has already been build"};
  }
  auto srcId = prog::sym::SourceId{++m_srcIdNum};
  if (!m_map.insert({srcId, SourceInfo{src, span}}).second) {
    assert(false); // Internal error, means we have duplicate source-ids.
  }
  return srcId;
}

auto SourceTableBuilder::build() -> SourceTable {
  if (m_build) {
    throw std::logic_error{"SourceTable has already been build"};
  }
  m_build = true;
  return SourceTable{std::move(m_map)};
}

} // namespace frontend::internal
