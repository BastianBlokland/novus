#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/const_decl_table.hpp"

namespace prog {

class Program;

namespace sym {

class ExecStmt final {
  friend auto execStmt(sym::ConstDeclTable consts, expr::NodePtr expr) -> ExecStmt;

public:
  ExecStmt()                        = delete;
  ExecStmt(const ExecStmt& rhs)     = delete;
  ExecStmt(ExecStmt&& rhs)          = default;
  ~ExecStmt()                       = default;

  auto operator=(const ExecStmt& rhs) -> ExecStmt& = delete;
  auto operator=(ExecStmt&& rhs) noexcept -> ExecStmt& = delete;

  [[nodiscard]] auto getConsts() const noexcept -> const sym::ConstDeclTable&;
  [[nodiscard]] auto getExpr() const noexcept -> const expr::Node&;

private:
  sym::ConstDeclTable m_consts;
  expr::NodePtr m_expr;

  ExecStmt(sym::ConstDeclTable consts, expr::NodePtr expr);
};

// Factories.
auto execStmt(sym::ConstDeclTable consts, expr::NodePtr expr) -> ExecStmt;

} // namespace sym

} // namespace prog
