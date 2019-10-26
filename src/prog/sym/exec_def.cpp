#include "prog/sym/exec_def.hpp"
#include "../expr/utilities.hpp"

namespace prog::sym {

ExecDef::ExecDef(
    sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args) :
    m_action{action}, m_consts{std::move(consts)}, m_args{std::move(args)} {}

auto execDef(
    const sym::ActionDeclTable& actionTable,
    sym::ActionId action,
    sym::ConstDeclTable consts,
    std::vector<expr::NodePtr> args) -> ExecDef {

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
  return ExecDef{action, std::move(consts), std::move(args)};
}

} // namespace prog::sym
