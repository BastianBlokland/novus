#include "prog/sym/exec_stmt.hpp"
#include "../expr/utilities.hpp"
#include "internal/conversion.hpp"
#include "prog/program.hpp"

namespace prog::sym {

ExecStmt::ExecStmt(
    sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args) :
    m_action{action}, m_consts{std::move(consts)}, m_args{std::move(args)} {}

auto ExecStmt::getActionId() const noexcept -> const ActionId& { return m_action; }

auto ExecStmt::getConsts() const noexcept -> const sym::ConstDeclTable& { return m_consts; }

auto ExecStmt::getArgs() const noexcept -> const std::vector<expr::NodePtr>& { return m_args; }

auto execStmt(
    const Program& prog,
    sym::ActionId action,
    sym::ConstDeclTable consts,
    std::vector<expr::NodePtr> args) -> ExecStmt {

  if (expr::anyNodeNull(args)) {
    throw std::invalid_argument{"Action execution cannot contain a null argument"};
  }
  const auto input = prog.getActionDecl(action).getInput();
  if (input.getCount() != args.size()) {
    throw std::invalid_argument{"Action execution contains incorrect number of arguments"};
  }

  // Apply conversions if necessary (and throw if types are incompatible).
  internal::applyConversions(prog, input, &args);

  return ExecStmt{action, std::move(consts), std::move(args)};
}

} // namespace prog::sym
