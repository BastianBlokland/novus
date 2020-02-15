#include "internal/define_exec_stmts.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/get_expr.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"
#include <cassert>

namespace frontend::internal {

DefineExecStmts::DefineExecStmts(Context* ctx) : m_ctx{ctx} {
  if (m_ctx == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto DefineExecStmts::visit(const parse::ExecStmtNode& n) -> void {
  auto consts        = prog::sym::ConstDeclTable{};
  auto visibleConsts = std::vector<prog::sym::ConstId>{};

  auto constBinder = ConstBinder{&consts, &visibleConsts, nullptr};
  auto getExpr     = GetExpr{m_ctx,
                         nullptr,
                         &constBinder,
                         prog::sym::TypeId::inferType(),
                         std::nullopt,
                         GetExpr::Flags::AllowActionCalls};
  n[0].accept(&getExpr);
  if (!getExpr.getValue()) {
    assert(m_ctx->hasErrors());
    return;
  }

  m_ctx->getProg()->addExecStmt(std::move(consts), std::move(getExpr.getValue()));
}

} // namespace frontend::internal
