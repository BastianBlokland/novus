#include "prog/program.hpp"

namespace prog {

auto Program::lookupType(const std::string& name) const -> std::optional<sym::TypeId> {
  return m_typeDecls.lookup(name);
}

auto Program::lookupFunc(const std::string& name, const sym::Input& input) const
    -> std::optional<sym::FuncId> {
  return m_funcDecls.lookup(name, input);
}

auto Program::lookupAction(const std::string& name, const sym::Input& input) const
    -> std::optional<sym::ActionId> {
  return m_actionDecls.lookup(name, input);
}

auto Program::declareUserFunc(std::string name, sym::FuncSig sig) {
  m_funcDecls.registerUser(std::move(name), std::move(sig));
}

auto Program::defineUserFunc(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr) {
  m_funcDefs.registerFunc(m_funcDecls, id, std::move(consts), std::move(expr));
}

auto Program::addExec(
    sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args) {
  return m_execs.push_back(sym::execDef(m_actionDecls, action, std::move(consts), std::move(args)));
}

} // namespace prog
