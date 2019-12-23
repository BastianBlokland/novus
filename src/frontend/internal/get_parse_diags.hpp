#pragma once
#include "internal/context.hpp"
#include "parse/node_visitor_deep.hpp"

namespace frontend::internal {

class GetParseDiags final : public parse::DeepNodeVisitor {
public:
  GetParseDiags() = delete;
  explicit GetParseDiags(Context* context);

  auto visit(const parse::ErrorNode& n) -> void override;

private:
  Context* m_context;
};

} // namespace frontend::internal
