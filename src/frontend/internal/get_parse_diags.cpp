#include "internal/get_parse_diags.hpp"
#include "frontend/diag_defs.hpp"

namespace frontend::internal {

GetParseDiags::GetParseDiags(const Source& src) : m_src{src} {}

auto GetParseDiags::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto GetParseDiags::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

auto GetParseDiags::visit(const parse::ErrorNode& n) -> void {
  parse::DeepNodeVisitor::visit(n);
  m_diags.push_back(errParseError(m_src, n));
}

} // namespace frontend::internal
