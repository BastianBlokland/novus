#include "gen_expr.hpp"
#include "prog/expr/nodes.hpp"
#include "utilities.hpp"
#include <stdexcept>
#include <vector>

namespace backend::internal {

GenExpr::GenExpr(const prog::Program& program, Builder* builder) :
    m_program{program}, m_builder{builder} {}

auto GenExpr::visit(const prog::expr::AssignExprNode& n) -> void {
  // Expression.
  genSubExpr(n[0]);

  // Assign op.
  const auto constId = getConstId(n.getConst());
  m_builder->addStoreConst(constId);
}

auto GenExpr::visit(const prog::expr::SwitchExprNode& n) -> void {
  const auto& conditions = n.getConditions();
  const auto& branches   = n.getBranches();

  // Generate labels for jumping.
  auto condBranchesLabels = std::vector<std::string>{};
  for (auto i = 0U; i < conditions.size(); ++i) {
    condBranchesLabels.push_back(m_builder->generateLabel());
  }
  const auto endLabel = m_builder->generateLabel();

  // Jump for the 'if' cases and fall-through for the else cases.
  for (auto i = 0U; i < conditions.size(); ++i) {
    genSubExpr(*conditions[i]);
    m_builder->addJumpIf(condBranchesLabels[i]);
  }

  // If all conditions where false we execute the else branch.
  genSubExpr(*branches.back());
  m_builder->addJump(endLabel);

  // Generate code for the 'if' branches.
  for (auto i = 0U; i < conditions.size(); ++i) {
    m_builder->label(condBranchesLabels[i]);
    genSubExpr(*branches[i]);

    // No need for a jump for the last.
    if (i != conditions.size() - 1) {
      m_builder->addJump(endLabel);
    }
  }

  m_builder->label(endLabel);
}

auto GenExpr::visit(const prog::expr::CallExprNode& n) -> void {
  // Push the arguments on the stack.
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    genSubExpr(n[i]);
  }

  // Either call the user function or the appropriate build-in instruction.
  const auto& funcDecl = m_program.getFuncDecl(n.getFunc());
  switch (funcDecl.getKind()) {
  case prog::sym::FuncKind::User:
    m_builder->addCall(getLabel(funcDecl.getId()));
    break;
  case prog::sym::FuncKind::NegateInt:
    m_builder->addNegInt();
    break;
  case prog::sym::FuncKind::AddInt:
    m_builder->addAddInt();
    break;
  case prog::sym::FuncKind::SubInt:
    m_builder->addSubInt();
    break;
  case prog::sym::FuncKind::MulInt:
    m_builder->addMulInt();
    break;
  case prog::sym::FuncKind::DivInt:
    m_builder->addDivInt();
    break;
  case prog::sym::FuncKind::CheckEqInt:
    m_builder->addCheckEqInt();
    break;
  case prog::sym::FuncKind::CheckNEqInt:
    m_builder->addCheckEqInt();
    m_builder->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckLeInt:
    m_builder->addCheckLeInt();
    break;
  case prog::sym::FuncKind::CheckLeEqInt:
    m_builder->addCheckGtInt();
    m_builder->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckGtInt:
    m_builder->addCheckGtInt();
    break;
  case prog::sym::FuncKind::CheckGtEqInt:
    m_builder->addCheckLeInt();
    m_builder->addLogicInvInt();
    break;
  case prog::sym::FuncKind::InvBool:
    m_builder->addLogicInvInt();
    break;
  case prog::sym::FuncKind::CheckEqBool:
    m_builder->addCheckEqInt();
    break;
  case prog::sym::FuncKind::CheckNEqBool:
    m_builder->addCheckEqInt();
    m_builder->addLogicInvInt();
    break;
  }
}

auto GenExpr::visit(const prog::expr::ConstExprNode& n) -> void {
  const auto constId = getConstId(n.getId());
  m_builder->addLoadConst(constId);
}

auto GenExpr::visit(const prog::expr::GroupExprNode& n) -> void {
  for (auto i = 0U; i < n.getChildCount(); ++i) {
    genSubExpr(n[i]);
  }
}

auto GenExpr::visit(const prog::expr::LitBoolNode& n) -> void {
  m_builder->addLoadLitInt(n.getVal() ? 1U : 0U);
}

auto GenExpr::visit(const prog::expr::LitIntNode& n) -> void {
  m_builder->addLoadLitInt(n.getVal());
}

auto GenExpr::genSubExpr(const prog::expr::Node& n) -> void {
  auto genExpr = GenExpr{m_program, m_builder};
  n.accept(&genExpr);
}

} // namespace backend::internal