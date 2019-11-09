#include "prog/program.hpp"
#include "prog/intrinsics.hpp"

namespace prog {

Program::Program() { registerIntrinsics(&m_typeDecls, &m_funcDecls, &m_actionDecls); }

auto Program::beginTypeDecls() const -> typeDeclIterator { return m_typeDecls.begin(); }

auto Program::endTypeDecls() const -> typeDeclIterator { return m_typeDecls.end(); }

auto Program::beginFuncDecls() const -> funcDeclIterator { return m_funcDecls.begin(); }

auto Program::endFuncDecls() const -> funcDeclIterator { return m_funcDecls.end(); }

auto Program::beginActionDecls() const -> actionDeclIterator { return m_actionDecls.begin(); }

auto Program::endActionDecls() const -> actionDeclIterator { return m_actionDecls.end(); }

auto Program::beginFuncDefs() const -> funcDefIterator { return m_funcDefs.begin(); }

auto Program::endFuncDefs() const -> funcDefIterator { return m_funcDefs.end(); }

auto Program::beginExecStmts() const -> execStmtIterator { return m_execStmts.begin(); }

auto Program::endExecStmts() const -> execStmtIterator { return m_execStmts.end(); }

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

auto Program::getTypeDecl(sym::TypeId id) const -> const sym::TypeDecl& { return m_typeDecls[id]; }

auto Program::getFuncDecl(sym::FuncId id) const -> const sym::FuncDecl& { return m_funcDecls[id]; }

auto Program::getActionDecl(sym::ActionId id) const -> const sym::ActionDecl& {
  return m_actionDecls[id];
}

auto Program::getFuncDef(sym::FuncId id) const -> const sym::FuncDef& { return m_funcDefs[id]; }

auto Program::declareUserFunc(std::string name, sym::FuncSig sig) -> void {
  m_funcDecls.registerUser(std::move(name), std::move(sig));
}

auto Program::defineUserFunc(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr)
    -> void {
  m_funcDefs.registerFunc(m_funcDecls, id, std::move(consts), std::move(expr));
}

auto Program::addExecStmt(
    sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args) -> void {
  return m_execStmts.push_back(
      sym::execStmt(m_actionDecls, action, std::move(consts), std::move(args)));
}

} // namespace prog
