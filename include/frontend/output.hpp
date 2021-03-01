#pragma once
#include "frontend/diag.hpp"
#include "prog/program.hpp"
#include "source_table.hpp"
#include <forward_list>
#include <utility>
#include <vector>

namespace frontend {

class Output final {
  friend auto buildOutput(
      std::unique_ptr<prog::Program> prog,
      std::forward_list<Source> importedSources,
      SourceTable sourceTable,
      std::vector<Diag> diags) -> Output;

public:
  using DiagIterator = typename std::vector<Diag>::const_iterator;

  Output() = delete;

  [[nodiscard]] auto isSuccess() const noexcept -> bool;
  [[nodiscard]] auto getProg() const noexcept -> const prog::Program&;
  [[nodiscard]] auto getImportedSources() const noexcept -> const std::forward_list<Source>&;
  [[nodiscard]] auto getSourceTable() const noexcept -> const SourceTable&;

  [[nodiscard]] auto beginDiags() const noexcept -> DiagIterator;
  [[nodiscard]] auto endDiags() const noexcept -> DiagIterator;

private:
  std::unique_ptr<prog::Program> m_prog;
  std::forward_list<Source> m_importedSources;
  SourceTable m_sourceTable;
  std::vector<Diag> m_diags;

  Output(
      std::unique_ptr<prog::Program> prog,
      std::forward_list<Source> importedSources,
      SourceTable sourceTable,
      std::vector<Diag> diags);
};

} // namespace frontend
