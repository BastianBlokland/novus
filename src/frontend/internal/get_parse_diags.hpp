#pragma once
#include "frontend/diag_defs.hpp"
#include "internal/context.hpp"
#include "parse/node_visitor_deep.hpp"

namespace frontend::internal {

class GetParseDiags final : public parse::DeepNodeVisitor {
public:
  GetParseDiags() = delete;
  explicit GetParseDiags(Context* ctx) : m_ctx{ctx} {
    if (m_ctx == nullptr) {
      throw std::invalid_argument{"Context cannot be null"};
    }
  }

  auto visit(const parse::ErrorNode& n) -> void override {
    parse::DeepNodeVisitor::visit(n);
    m_ctx->reportDiag(errParseError, n);
  }

private:
  Context* m_ctx;
};

} // namespace frontend::internal
