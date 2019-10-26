#pragma once
#include "prog/expr/node.hpp"
#include "prog/sym/action_decl_table.hpp"
#include "prog/sym/action_id.hpp"
#include "prog/sym/const_decl_table.hpp"

namespace prog::sym {

class ExecDef final {
  friend auto execDef(
      const sym::ActionDeclTable& actionTable,
      sym::ActionId action,
      sym::ConstDeclTable consts,
      std::vector<expr::NodePtr> args) -> ExecDef;

public:
  ExecDef()                       = delete;
  ExecDef(const ExecDef& rhs)     = delete;
  ExecDef(ExecDef&& rhs) noexcept = default;
  ~ExecDef()                      = default;

  auto operator=(const ExecDef& rhs) -> ExecDef& = delete;
  auto operator=(ExecDef&& rhs) noexcept -> ExecDef& = delete;

private:
  sym::ActionId m_action;
  sym::ConstDeclTable m_consts;
  std::vector<expr::NodePtr> m_args;

  ExecDef(sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args);
};

// Factories.
auto execDef(
    const sym::ActionDeclTable& actionTable,
    sym::ActionId action,
    sym::ConstDeclTable consts,
    std::vector<expr::NodePtr> args) -> ExecDef;

} // namespace prog::sym
