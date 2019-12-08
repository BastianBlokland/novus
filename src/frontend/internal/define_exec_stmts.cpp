#include "internal/define_exec_stmts.hpp"
#include "frontend/diag_defs.hpp"
#include "internal/get_expr.hpp"
#include "internal/utilities.hpp"
#include "parse/nodes.hpp"

namespace frontend::internal {

DefineExecStmts::DefineExecStmts(const Source& src, prog::Program* prog) :
    m_src{src}, m_prog{prog} {}

auto DefineExecStmts::hasErrors() const noexcept -> bool { return !m_diags.empty(); }

auto DefineExecStmts::getDiags() const noexcept -> const std::vector<Diag>& { return m_diags; }

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
  const auto& action     = m_prog->lookupAction(actionName, prog::sym::Input{argTypes}, -1);
  if (action) {
    m_prog->addExecStmt(action.value(), std::move(consts), std::move(args));
    return;
  }

  if (m_prog->lookupActions(actionName).empty()) {
    m_diags.push_back(errUndeclaredAction(m_src, actionName, n.getAction().getSpan()));
  } else {
    auto argTypeNames = std::vector<std::string>{};
    for (const auto& argType : argTypes) {
      argTypeNames.push_back(m_prog->getTypeDecl(argType).getName());
    }
    m_diags.push_back(errUndeclaredActionOverload(m_src, actionName, argTypeNames, n.getSpan()));
  }
}

auto DefineExecStmts::getExpr(
    const parse::Node& n,
    prog::sym::ConstDeclTable* consts,
    std::vector<prog::sym::ConstId>* visibleConsts,
    prog::sym::TypeId typeHint) -> prog::expr::NodePtr {

  auto getExpr = GetExpr{m_src, m_prog, consts, visibleConsts, typeHint};
  n.accept(&getExpr);
  m_diags.insert(m_diags.end(), getExpr.getDiags().begin(), getExpr.getDiags().end());
  return std::move(getExpr.getValue());
}

} // namespace frontend::internal
