#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "parse/node_error.hpp"
#include "parse/node_visitor_reqursive.hpp"
#include <vector>

namespace frontend::visitors {

class GetParseDiags final : public parse::RecursiveNodeVisitor {
public:
  GetParseDiags() = delete;
  explicit GetParseDiags(const Source& src) : m_src{src}, m_diags{} {}

  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

  auto visit(const parse::ErrorNode& n) -> void override {
    parse::RecursiveNodeVisitor::visit(n);

    m_diags.push_back(error(m_src, n.getMessage(), n.getSpan()));
  }

private:
  const Source& m_src;
  std::vector<Diag> m_diags;
};

} // namespace frontend::visitors
