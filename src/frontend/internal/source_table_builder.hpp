#pragma once
#include "frontend/source_table.hpp"
#include "input/span.hpp"

namespace frontend::internal {

class SourceTableBuilder final {
public:
  SourceTableBuilder() noexcept;
  SourceTableBuilder(const SourceTableBuilder& rhs)     = delete;
  SourceTableBuilder(SourceTableBuilder&& rhs) noexcept = default;
  ~SourceTableBuilder()                                 = default;

  auto operator=(const SourceTableBuilder& rhs) -> SourceTableBuilder& = delete;
  auto operator=(SourceTableBuilder&& rhs) noexcept -> SourceTableBuilder& = default;

  [[nodiscard]] auto add(const Source* src, input::Span span) -> prog::sym::SourceId;
  [[nodiscard]] auto build() -> SourceTable;

private:
  bool m_build;
  SourceTable::Map m_map;
  unsigned int m_srcIdNum;
};

} // namespace frontend::internal
