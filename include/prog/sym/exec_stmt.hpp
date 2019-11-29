#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/action_decl_table.hpp"
#include "prog/sym/action_id.hpp"
#include "prog/sym/const_decl_table.hpp"

namespace prog {

class Program;

namespace sym {

class ExecStmt final {
  friend auto execStmt(
      const Program& program,
      sym::ActionId action,
      sym::ConstDeclTable consts,
      std::vector<expr::NodePtr> args) -> ExecStmt;

public:
  ExecStmt()                        = delete;
  ExecStmt(const ExecStmt& rhs)     = delete;
  ExecStmt(ExecStmt&& rhs) noexcept = default;
  ~ExecStmt()                       = default;

  auto operator=(const ExecStmt& rhs) -> ExecStmt& = delete;
  auto operator=(ExecStmt&& rhs) noexcept -> ExecStmt& = delete;

  [[nodiscard]] auto getActionId() const noexcept -> const ActionId&;
  [[nodiscard]] auto getConsts() const noexcept -> const sym::ConstDeclTable&;
  [[nodiscard]] auto getArgs() const noexcept -> const std::vector<expr::NodePtr>&;

private:
  sym::ActionId m_action;
  sym::ConstDeclTable m_consts;
  std::vector<expr::NodePtr> m_args;

  ExecStmt(sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args);
};

// Factories.
auto execStmt(
    const Program& program,
    sym::ActionId action,
    sym::ConstDeclTable consts,
    std::vector<expr::NodePtr> args) -> ExecStmt;

} // namespace sym

} // namespace prog
