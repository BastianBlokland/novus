#pragma once
#include "filesystem.hpp"
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "internal/source_table_builder.hpp"
#include "parse/node_visitor_optional.hpp"
#include <forward_list>
#include <optional>
#include <string>
#include <vector>

namespace frontend::internal {

class ImportSources final : public parse::OptionalNodeVisitor {
  using Path = filesystem::path;

public:
  ImportSources() = delete;
  ImportSources(
      const Source& mainSource,
      SourceTableBuilder& sourceTableBuilder,
      const std::vector<Path>& searchPaths,
      std::forward_list<Source>* importedSources,
      std::vector<Diag>* diags);
  ImportSources(
      const Source& mainSource,
      const Source& currentSource,
      SourceTableBuilder& sourceTableBuilder,
      const std::vector<Path>& searchPaths,
      std::forward_list<Source>* importedSources,
      std::vector<Diag>* diags);

  auto visit(const parse::ImportStmtNode& n) -> void override;

private:
  const Source& m_mainSource;
  const Source& m_currentSource;
  SourceTableBuilder& m_sourceTableBuilder;
  const std::vector<Path>& m_searchPaths;
  std::forward_list<Source>* m_importedSources;
  std::vector<Diag>* m_diags;

  [[nodiscard]] auto alreadyImportedAbsPath(const Path& file) const -> bool;
  [[nodiscard]] auto alreadyImportedRelPath(const Path& file) const -> bool;

  [[nodiscard]] auto import(const Path& file, input::Span span) const -> bool;

  [[nodiscard]] auto importFromDir(const Path& searchPath, const Path& file) const -> bool;
};

} // namespace frontend::internal
