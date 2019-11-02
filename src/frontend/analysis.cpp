#include "frontend/analysis.hpp"
#include "visitors/get_parse_diags.hpp"
#include <vector>

namespace frontend {

static auto getParseDiags(const Source& src) {
  auto visitor = visitors::GetParseDiags{src};
  src.accept(&visitor);
  return visitor.getDiags();
}

auto analyze(const Source& src) -> Output {
  // Check source for any parse errors.
  auto parseDiags = getParseDiags(src);
  if (!parseDiags.empty()) {
    return buildOutput(nullptr, std::move(parseDiags));
  }

  return buildOutput(nullptr, std::vector<Diag>{});
}

} // namespace frontend
