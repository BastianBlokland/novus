#include "internal/import_sources.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/source_table_builder.hpp"
#include "internal/utilities.hpp"
#include "parse/node_stmt_import.hpp"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <system_error>

namespace frontend::internal {

ImportSources::ImportSources(
    const Source& mainSource,
    SourceTableBuilder& sourceTableBuilder,
    const std::vector<Path>& searchPaths,
    std::forward_list<Source>* importedSources,
    std::vector<Diag>* diags) :
    ImportSources(mainSource, mainSource, sourceTableBuilder, searchPaths, importedSources, diags) {
}

ImportSources::ImportSources(
    const Source& mainSource,
    const Source& currentSource,
    SourceTableBuilder& sourceTableBuilder,
    const std::vector<Path>& searchPaths,
    std::forward_list<Source>* importedSources,
    std::vector<Diag>* diags) :
    m_mainSource{mainSource},
    m_currentSource{currentSource},
    m_sourceTableBuilder{sourceTableBuilder},
    m_searchPaths{searchPaths},
    m_importedSources{importedSources},
    m_diags{diags} {

  if (m_importedSources == nullptr) {
    throw std::invalid_argument{"ImportedSources output list cannot be null"};
  }
  if (m_diags == nullptr) {
    throw std::invalid_argument{"Diagnostics output vector cannot be null"};
  }
}

auto ImportSources::visit(const parse::ImportStmtNode& n) -> void {
  if (!import(n.getPathString(), n.getPath().getSpan())) {
    assert(!m_diags->empty());
    return;
  }
}

auto ImportSources::alreadyImportedAbsPath(const Path& file) const -> bool {
  assert(file.is_absolute());
  assert(!m_mainSource.getPath() || m_mainSource.getPath()->is_absolute());

  if (m_mainSource.getPath() && file == *m_mainSource.getPath()) {
    return true;
  }
  if (std::any_of(m_importedSources->begin(), m_importedSources->end(), [&file](const Source& src) {
        assert(src.getPath());
        assert(src.getPath()->is_absolute());
        return file == *src.getPath();
      })) {
    return true;
  }

  return false;
}

auto ImportSources::alreadyImportedRelPath(const Path& file) const -> bool {
  assert(file.is_relative());

  if (m_currentSource.getPath()) {
    if (alreadyImportedAbsPath(m_currentSource.getPath()->parent_path() / file)) {
      return true;
    }
  }
  for (const auto& searchPath : m_searchPaths) {
    if (alreadyImportedAbsPath(searchPath / file)) {
      return true;
    }
  }
  return false;
}

auto ImportSources::import(const Path& file, input::Span span) const -> bool {

  if (file.is_absolute() && importAbsPath(file)) {
    return true;
  }
  if (file.is_relative() && importRelPath(file)) {
    return true;
  }

  const auto srcId = m_sourceTableBuilder.add(&m_currentSource, span);
  m_diags->push_back(errUnresolvedImport(srcId, file.string()));
  return false;
}

auto ImportSources::importRelPath(const Path& file) const -> bool {
  assert(file.is_relative());

  if (alreadyImportedRelPath(file)) {
    return true;
  }
  if (m_currentSource.getPath()) {
    if (importAbsPath(m_currentSource.getPath()->parent_path() / file)) {
      return true;
    }
  }
  for (const auto& searchPath : m_searchPaths) {
    if (importAbsPath(searchPath / file)) {
      return true;
    }
  }
  return false;
}

auto ImportSources::importAbsPath(const Path& file) const -> bool {
  assert(file.is_absolute());

  std::error_code err;
  Path canonicalPath = filesystem::canonical(file, err);
  if (err) {
    return false;
  }

  if (alreadyImportedAbsPath(canonicalPath)) {
    return true;
  }

  auto fs = std::ifstream{canonicalPath.string()};
  if (!fs.good()) {
    return false;
  }

  m_importedSources->push_front(frontend::buildSource(
      canonicalPath.filename().string(),
      canonicalPath,
      std::istreambuf_iterator<char>{fs},
      std::istreambuf_iterator<char>{}));

  auto importNested = ImportSources{
      m_mainSource,
      m_importedSources->front(),
      m_sourceTableBuilder,
      m_searchPaths,
      m_importedSources,
      m_diags};
  m_importedSources->front().accept(&importNested);
  return true;
}

} // namespace frontend::internal
