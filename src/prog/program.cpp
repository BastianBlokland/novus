#include "prog/program.hpp"
#include "prog/intrinsics.hpp"

namespace prog {

Program::Program() { registerIntrinsics(&m_typeDecls, &m_funcDecls, &m_actionDecls); }

auto Program::beginTypeDecls() const -> typeDeclIterator { return m_typeDecls.begin(); }

auto Program::endTypeDecls() const -> typeDeclIterator { return m_typeDecls.end(); }

auto Program::beginFuncDecls() const -> funcDeclIterator { return m_funcDecls.begin(); }

auto Program::endFuncDecls() const -> funcDeclIterator { return m_funcDecls.end(); }

auto Program::lookupType(const std::string& name) const -> std::optional<sym::TypeId> {
  return m_typeDecls.lookup(name);
}

auto Program::lookupFunc(const std::string& name, const sym::Input& input) const
    -> std::optional<sym::FuncId> {
  return m_funcDecls.lookup(name, input);
}

auto Program::lookupFuncs(const std::string& name) const -> std::vector<sym::FuncId> {
  return m_funcDecls.lookup(name);
}

auto Program::lookupAction(const std::string& name, const sym::Input& input) const
    -> std::optional<sym::ActionId> {
  return m_actionDecls.lookup(name, input);
}

auto Program::lookupActions(const std::string& name) const -> std::vector<sym::ActionId> {
  return m_actionDecls.lookup(name);
}

auto Program::declareUserFunc(std::string name, sym::FuncSig sig) -> void {
  m_funcDecls.registerUser(std::move(name), std::move(sig));
}

auto Program::defineUserFunc(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr)
    -> void {
  m_funcDefs.registerFunc(m_funcDecls, id, std::move(consts), std::move(expr));
}

auto Program::addExec(
    sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args) -> void {
  return m_execs.push_back(sym::execDef(m_actionDecls, action, std::move(consts), std::move(args)));
}

} // namespace prog
