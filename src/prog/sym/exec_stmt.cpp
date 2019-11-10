#include "prog/sym/exec_stmt.hpp"
#include "../expr/utilities.hpp"

namespace prog::sym {

ExecStmt::ExecStmt(
    sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args) :
    m_action{action}, m_consts{std::move(consts)}, m_args{std::move(args)} {}

auto ExecStmt::getActionId() const noexcept -> const ActionId& { return m_action; }

auto ExecStmt::getConsts() const noexcept -> const sym::ConstDeclTable& { return m_consts; }

auto ExecStmt::getArgs() const noexcept -> const std::vector<expr::NodePtr>& { return m_args; }

auto execStmt(
    const sym::ActionDeclTable& actionTable,
    sym::ActionId action,
    sym::ConstDeclTable consts,
    std::vector<expr::NodePtr> args) -> ExecStmt {

  if (expr::anyNodeNull(args)) {
    throw std::invalid_argument{"Action execution cannot contain a null argument"};
  }
  const auto input = actionTable[action].getInput();
  if (input.getCount() != args.size()) {
    throw std::invalid_argument{"Action execution contains incorrect number of arguments"};
  }
  for (auto i = 0U; i < input.getCount(); ++i) {
    if (args[i]->getType() != input[i]) {
      throw std::invalid_argument{
          "Action execution contains argument who's type doesn't match action input"};
    }
  }
  return ExecStmt{action, std::move(consts), std::move(args)};
}

} // namespace prog::sym
