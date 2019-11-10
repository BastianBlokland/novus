#pragma once
#include "frontend/diag.hpp"
#include "frontend/source.hpp"
#include "parse/node_visitor_deep.hpp"

namespace frontend::internal {

class GetParseDiags final : public parse::DeepNodeVisitor {
public:
  GetParseDiags() = delete;
  explicit GetParseDiags(const Source& src);

  [[nodiscard]] auto hasErrors() const noexcept -> bool;

  [[nodiscard]] auto getDiags() const noexcept -> const std::vector<Diag>&;

  auto visit(const parse::ErrorNode& n) -> void override;

private:
  const Source& m_src;
  std::vector<Diag> m_diags;
};

} // namespace frontend::internal
