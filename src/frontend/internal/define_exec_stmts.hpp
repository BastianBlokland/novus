#pragma once
#include "internal/context.hpp"
#include "parse/node_visitor_optional.hpp"

namespace frontend::internal {

class DefineExecStmts final : public parse::OptionalNodeVisitor {
public:
  DefineExecStmts() = delete;
  explicit DefineExecStmts(Context* ctx);

  auto visit(const parse::ExecStmtNode& n) -> void override;

private:
  Context* m_ctx;
};

} // namespace frontend::internal
