#pragma once
#include "frontend/diag.hpp"
#include "prog/program.hpp"
#include <utility>
#include <vector>

namespace frontend {

class Output final {
  friend auto buildOutput(std::unique_ptr<prog::Program> prog, std::vector<Diag> diags) -> Output;

public:
  using diagIterator = typename std::vector<Diag>::const_iterator;

  Output() = delete;

  [[nodiscard]] auto isSuccess() const noexcept -> bool;
  [[nodiscard]] auto getProg() const noexcept -> const prog::Program&;

  [[nodiscard]] auto beginDiags() const noexcept -> diagIterator;
  [[nodiscard]] auto endDiags() const noexcept -> diagIterator;

private:
  std::unique_ptr<prog::Program> m_prog;
  std::vector<Diag> m_diags;

  Output(std::unique_ptr<prog::Program> prog, std::vector<Diag> diags);
};

auto buildOutput(std::unique_ptr<prog::Program> prog, std::vector<Diag> diags) -> Output;

} // namespace frontend
