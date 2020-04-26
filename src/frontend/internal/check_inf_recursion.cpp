#include "internal/check_inf_recursion.hpp"
#include "prog/expr/nodes.hpp"

namespace frontend::internal {

CheckInfRecursion::CheckInfRecursion(const Context& ctx, prog::sym::FuncId func) :
    m_ctx{ctx}, m_func{func}, m_infRec{false} {}

auto CheckInfRecursion::isInfRecursion() const -> bool { return m_infRec; }

auto CheckInfRecursion::visit(const prog::expr::AssignExprNode& n) -> void {
  m_infRec |= exprLeadsToInfRec(n[0]);
}

auto CheckInfRecursion::visit(const prog::expr::SwitchExprNode& n) -> void {
  // In theory this includes false positives as not all conditions might be executed, however
  // depending on that behaviour is considered poor design.
  for (const auto& cond : n.getConditions()) {
    m_infRec |= exprLeadsToInfRec(*cond);
  }

  // Check if all branches lead to infinite recursion.
  auto allBranchsInfRec = true;
  for (const auto& branch : n.getBranches()) {
    allBranchsInfRec &= exprLeadsToInfRec(*branch);
  }
  m_infRec |= allBranchsInfRec;
}

auto CheckInfRecursion::visit(const prog::expr::CallExprNode& n) -> void {
  if (n.getFunc() == m_func && !n.isLazy()) {
    m_infRec = true;
  }
  for (auto i = 0U; i != n.getChildCount(); ++i) {
    m_infRec |= exprLeadsToInfRec(n[i]);
  }
}

auto CheckInfRecursion::visit(const prog::expr::CallDynExprNode& n) -> void {
  for (auto i = 0U; i != n.getChildCount(); ++i) {
    m_infRec |= exprLeadsToInfRec(n[i]);
  }
}

auto CheckInfRecursion::visit(const prog::expr::ClosureNode& n) -> void {
  for (auto i = 0U; i != n.getChildCount(); ++i) {
    m_infRec |= exprLeadsToInfRec(n[i]);
  }
}

auto CheckInfRecursion::visit(const prog::expr::FieldExprNode& n) -> void {
  m_infRec |= exprLeadsToInfRec(n[0]);
}

auto CheckInfRecursion::visit(const prog::expr::GroupExprNode& n) -> void {
  for (auto i = 0U; i != n.getChildCount(); ++i) {
    m_infRec |= exprLeadsToInfRec(n[i]);
  }
}

auto CheckInfRecursion::visit(const prog::expr::UnionCheckExprNode& n) -> void {
  m_infRec |= exprLeadsToInfRec(n[0]);
}

auto CheckInfRecursion::visit(const prog::expr::UnionGetExprNode& n) -> void {
  m_infRec |= exprLeadsToInfRec(n[0]);
}

auto CheckInfRecursion::exprLeadsToInfRec(const prog::expr::Node& expr) const -> bool {
  auto visitor = CheckInfRecursion{m_ctx, m_func};
  expr.accept(&visitor);
  return visitor.m_infRec;
}

} // namespace frontend::internal
