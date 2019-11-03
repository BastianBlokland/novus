#pragma once
#include "frontend/diag_defs.hpp"
#include "frontend/source.hpp"
#include "parse/node_error.hpp"
#include "parse/node_visitor_deep.hpp"
#include <vector>

namespace frontend::visitors {

class GetParseDiags final : public parse::DeepNodeVisitor {
public:
  GetParseDiags() = delete;
  explicit GetParseDiags(const Source& src) : m_src{src}, m_diags{} {}

  [[nodiscard]] auto hasErrors() const noexcept -> bool { return !m_diags.empty(); }

  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

  auto visit(const parse::ErrorNode& n) -> void override {
    parse::DeepNodeVisitor::visit(n);

    m_diags.push_back(errParseError(m_src, n));
  }

private:
  const Source& m_src;
  std::vector<Diag> m_diags;
};

} // namespace frontend::visitors
