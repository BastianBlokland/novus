#include "prog/sym/exec_stmt.hpp"

namespace prog::sym {

ExecStmt::ExecStmt(sym::ConstDeclTable consts, expr::NodePtr expr) :
    m_consts{std::move(consts)}, m_expr{std::move(expr)} {}

auto ExecStmt::getConsts() const noexcept -> const sym::ConstDeclTable& { return m_consts; }

auto ExecStmt::getExpr() const noexcept -> const expr::Node& { return *m_expr; }

auto execStmt(sym::ConstDeclTable consts, expr::NodePtr expr) -> ExecStmt {
  if (expr == nullptr) {
    throw std::invalid_argument{"Execution cannot contain a null expression"};
  }
  return ExecStmt{std::move(consts), std::move(expr)};
}

} // namespace prog::sym
