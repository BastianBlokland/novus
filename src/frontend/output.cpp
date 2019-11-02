#include "frontend/output.hpp"
#include <stdexcept>
#include <utility>

namespace frontend {

Output::Output(std::unique_ptr<Program> prog, std::vector<Diag> diags) :
    m_prog{std::move(prog)}, m_diags{std::move(diags)} {}

auto Output::isSuccess() const noexcept -> bool { return m_prog != nullptr; }

auto Output::getProg() const noexcept -> Program* { return m_prog.get(); }

auto Output::beginDiags() const noexcept -> diagIterator { return m_diags.begin(); }

auto Output::endDiags() const noexcept -> diagIterator { return m_diags.end(); }

auto buildOutput(std::unique_ptr<Program> prog, std::vector<Diag> diags) -> Output {
  if (prog == nullptr && diags.empty()) {
    throw std::logic_error(
        "If not program could be constructed, at least one diagnostic should be present");
  }
  return Output{std::move(prog), std::move(diags)};
}

} // namespace frontend
