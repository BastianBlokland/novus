#include "internal/get_parse_diags.hpp"
#include "frontend/diag_defs.hpp"

namespace frontend::internal {

GetParseDiags::GetParseDiags(Context* context) : m_context{context} {
  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto GetParseDiags::visit(const parse::ErrorNode& n) -> void {
  parse::DeepNodeVisitor::visit(n);
  m_context->reportDiag(errParseError(m_context->getSrc(), n));
}

} // namespace frontend::internal
