#include "frontend/output.hpp"
#include <stdexcept>
#include <utility>

namespace frontend {

Output::Output(
    std::unique_ptr<prog::Program> prog,
    std::forward_list<Source> importedSources,
    std::vector<Diag> diags) :
    m_prog{std::move(prog)},
    m_importedSources{std::move(importedSources)},
    m_diags{std::move(diags)} {}

auto Output::isSuccess() const noexcept -> bool { return m_prog != nullptr; }

auto Output::getProg() const noexcept -> const prog::Program& { return *m_prog; }

auto Output::getImportedSources() const noexcept -> const std::forward_list<Source>& {
  return m_importedSources;
}

auto Output::beginDiags() const noexcept -> DiagIterator { return m_diags.begin(); }

auto Output::endDiags() const noexcept -> DiagIterator { return m_diags.end(); }

auto buildOutput(
    std::unique_ptr<prog::Program> prog,
    std::forward_list<Source> importedSources,
    std::vector<Diag> diags) -> Output {

  if (prog == nullptr && diags.empty()) {
    throw std::logic_error{
        "If not program could be constructed, at least one diagnostic should be present"};
  }
  return Output{std::move(prog), std::move(importedSources), std::move(diags)};
}

} // namespace frontend
