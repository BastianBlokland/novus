#include "internal/define_exec_stmts.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/get_expr.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DefineExecStmts::DefineExecStmts(Context* context) : m_context{context} {
  if (m_context == nullptr) {
    throw std::invalid_argument{"Context cannot be null"};
  }
}

auto DefineExecStmts::visit(const parse::ExecStmtNode& n) -> void {
  auto isValid       = true;
  auto consts        = prog::sym::ConstDeclTable{};
  auto visibleConsts = std::vector<prog::sym::ConstId>{};
  auto argTypes      = std::vector<prog::sym::TypeId>{};
  auto args          = std::vector<prog::expr::NodePtr>{};

  for (auto i = 0U; i < n.getChildCount(); ++i) {
    auto arg = getExpr(n[i], &consts, &visibleConsts, prog::sym::TypeId::inferType());
    if (arg) {
      argTypes.push_back(arg->getType());
      args.push_back(std::move(arg));
    } else {
      isValid = false;
    }
  }
  if (!isValid) {
    return;
  }

  const auto& actionName = getName(n.getAction());
  const auto& action =
      m_context->getProg()->lookupAction(actionName, prog::sym::TypeSet{argTypes}, -1);
  if (action) {
    m_context->getProg()->addExecStmt(action.value(), std::move(consts), std::move(args));
    return;
  }

  if (m_context->getProg()->lookupActions(actionName).empty()) {
    m_context->reportDiag(
        errUndeclaredAction(m_context->getSrc(), actionName, n.getAction().getSpan()));
  } else {
    auto argTypeNames = std::vector<std::string>{};
    for (const auto& argType : argTypes) {
      argTypeNames.push_back(getName(m_context, argType));
    }
    m_context->reportDiag(
        errUndeclaredActionOverload(m_context->getSrc(), actionName, argTypeNames, n.getSpan()));
  }
}

auto DefineExecStmts::getExpr(
    const parse::Node& n,
    prog::sym::ConstDeclTable* consts,
    std::vector<prog::sym::ConstId>* visibleConsts,
    prog::sym::TypeId typeHint) -> prog::expr::NodePtr {

  auto constBinder = ConstBinder{consts, visibleConsts, nullptr};
  auto getExpr     = GetExpr{m_context, nullptr, &constBinder, typeHint};
  n.accept(&getExpr);
  return std::move(getExpr.getValue());
}

} // namespace frontend::internal
