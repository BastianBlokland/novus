#include "internal/import_sources.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/utilities.hpp"
#include "parse/node_stmt_import.hpp"
#include <algorithm>
#include <cassert>
#include <fstream>

namespace frontend::internal {

ImportSources::ImportSources(
    const Source& mainSource,
    const std::vector<Path>& searchPaths,
    std::forward_list<Source>* importedSources,
    std::vector<Diag>* diags) :
    ImportSources(mainSource, mainSource, searchPaths, importedSources, diags) {}

ImportSources::ImportSources(
    const Source& mainSource,
    const Source& currentSource,
    const std::vector<Path>& searchPaths,
    std::forward_list<Source>* importedSources,
    std::vector<Diag>* diags) :
    m_mainSource{mainSource},
    m_currentSource{currentSource},
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

auto ImportSources::import(const Path& file, input::Span span) const -> bool {

  // Check if this file is same as our main source.
  if (m_mainSource.getPath()) {
    if (file.filename() == m_mainSource.getPath()->filename()) {
      return true;
    }
  }

  // Check if we've already imported this file.
  if (std::any_of(m_importedSources->begin(), m_importedSources->end(), [&file](const Source& src) {
        return file.filename() == src.getPath()->filename();
      })) {
    return true;
  }

  // First attempt from the local directory of the current source.
  if (m_currentSource.getPath()) {
    if (importFromDir(m_currentSource.getPath()->parent_path(), file)) {
      return true;
    }
  }

  // Otherwise try in all the searchpaths.
  for (const auto& searchPath : m_searchPaths) {
    if (importFromDir(searchPath, file)) {
      return true;
    }
  }

  m_diags->push_back(errUnresolvedImport(m_currentSource, file.string(), span));
  return false;
}

auto ImportSources::importFromDir(const Path& searchPath, const Path& file) const -> bool {
  const auto fullPath = searchPath / file;
  auto fs             = std::ifstream{fullPath.string()};
  if (!fs.good()) {
    return false;
  }

  m_importedSources->push_front(frontend::buildSource(
      file.filename().string(),
      filesystem::canonical(fullPath),
      std::istreambuf_iterator<char>{fs},
      std::istreambuf_iterator<char>{}));

  auto importNested = ImportSources{
      m_mainSource, m_importedSources->front(), m_searchPaths, m_importedSources, m_diags};
  m_importedSources->front().accept(&importNested);
  return true;
}

} // namespace frontend::internal
