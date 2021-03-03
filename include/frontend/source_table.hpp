#pragma once
#include "input/span.hpp"
#include "prog/sym/source_id.hpp"
#include "prog/sym/source_id_hasher.hpp"
#include "source.hpp"
#include <unordered_map>

namespace frontend {

namespace internal {
class SourceTableBuilder;
}

struct SourceInfo {
  const Source* source;
  input::Span span;

  [[nodiscard]] auto getPath() const noexcept -> std::optional<filesystem::path> {
    return source->getPath();
  }

  [[nodiscard]] auto getStart() const noexcept -> input::TextPos {
    return source->getTextPos(span.getStart());
  }

  [[nodiscard]] auto getEnd() const noexcept -> input::TextPos {
    return source->getTextPos(span.getStart());
  }
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
