#pragma once
#include "input/span.hpp"
#include "prog/sym/source_id.hpp"
#include "prog/sym/source_id_hasher.hpp"
#include <unordered_map>

namespace frontend {

class Source;
namespace internal {
class SourceTableBuilder;
}

struct SourceInfo {
  const Source* source;
  input::Span span;
};

class SourceTable final {
  friend class internal::SourceTableBuilder;

public:
  SourceTable()                           = delete;
  SourceTable(const SourceTable& rhs)     = delete;
  SourceTable(SourceTable&& rhs) noexcept = default;
  ~SourceTable()                          = default;

  auto operator=(const SourceTable& rhs) -> SourceTable& = delete;
  auto operator=(SourceTable&& rhs) noexcept -> SourceTable& = default;

  [[nodiscard]] auto operator[](prog::sym::SourceId source) const -> SourceInfo;

private:
  using SrcId       = prog::sym::SourceId;
  using SrcIdHasher = prog::sym::SourceIdHasher;
  using Map         = std::unordered_map<SrcId, SourceInfo, SrcIdHasher>;

  Map m_map;

  SourceTable(Map map);
};

} // namespace frontend
