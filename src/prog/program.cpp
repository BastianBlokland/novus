#include "prog/program.hpp"
#include "prog/operator.hpp"
#include "prog/sym/action_kind.hpp"

namespace prog {

Program::Program() {
  // Register build-in types.
  const auto intT    = m_typeDecls.registerType(sym::TypeKind::Int, "int");
  const auto boolT   = m_typeDecls.registerType(sym::TypeKind::Bool, "bool");
  const auto stringT = m_typeDecls.registerType(sym::TypeKind::String, "string");

  // Register build-in unary int operators.
  m_funcDecls.registerFunc(
      sym::FuncKind::NegateInt, getFuncName(Operator::Minus), sym::FuncSig{sym::Input{intT}, intT});

  // Register build-in binary int operators.
  m_funcDecls.registerFunc(
      sym::FuncKind::AddInt,
      getFuncName(Operator::Plus),
      sym::FuncSig{sym::Input{intT, intT}, intT});
  m_funcDecls.registerFunc(
      sym::FuncKind::SubInt,
      getFuncName(Operator::Minus),
      sym::FuncSig{sym::Input{intT, intT}, intT});
  m_funcDecls.registerFunc(
      sym::FuncKind::MulInt,
      getFuncName(Operator::Star),
      sym::FuncSig{sym::Input{intT, intT}, intT});
  m_funcDecls.registerFunc(
      sym::FuncKind::DivInt,
      getFuncName(Operator::Slash),
      sym::FuncSig{sym::Input{intT, intT}, intT});
  m_funcDecls.registerFunc(
      sym::FuncKind::RemInt,
      getFuncName(Operator::Rem),
      sym::FuncSig{sym::Input{intT, intT}, intT});
  m_funcDecls.registerFunc(
      sym::FuncKind::CheckEqInt,
      getFuncName(Operator::EqEq),
      sym::FuncSig{sym::Input{intT, intT}, boolT});
  m_funcDecls.registerFunc(
      sym::FuncKind::CheckNEqInt,
      getFuncName(Operator::BangEq),
      sym::FuncSig{sym::Input{intT, intT}, boolT});
  m_funcDecls.registerFunc(
      sym::FuncKind::CheckLeInt,
      getFuncName(Operator::Le),
      sym::FuncSig{sym::Input{intT, intT}, boolT});
  m_funcDecls.registerFunc(
      sym::FuncKind::CheckLeEqInt,
      getFuncName(Operator::LeEq),
      sym::FuncSig{sym::Input{intT, intT}, boolT});
  m_funcDecls.registerFunc(
      sym::FuncKind::CheckGtInt,
      getFuncName(Operator::Gt),
      sym::FuncSig{sym::Input{intT, intT}, boolT});
  m_funcDecls.registerFunc(
      sym::FuncKind::CheckGtEqInt,
      getFuncName(Operator::GtEq),
      sym::FuncSig{sym::Input{intT, intT}, boolT});

  // Register build-in unary bool operators.
  m_funcDecls.registerFunc(
      sym::FuncKind::InvBool, getFuncName(Operator::Bang), sym::FuncSig{sym::Input{boolT}, boolT});

  // Register build-in binary bool operators.
  m_funcDecls.registerFunc(
      sym::FuncKind::CheckEqBool,
      getFuncName(Operator::EqEq),
      sym::FuncSig{sym::Input{boolT, boolT}, boolT});
  m_funcDecls.registerFunc(
      sym::FuncKind::CheckNEqBool,
      getFuncName(Operator::BangEq),
      sym::FuncSig{sym::Input{boolT, boolT}, boolT});

  // Register build-in binary string operators.
  m_funcDecls.registerFunc(
      sym::FuncKind::AddString,
      getFuncName(Operator::Plus),
      sym::FuncSig{sym::Input{stringT, stringT}, stringT});

  // Register build-in conversions.
  m_funcDecls.registerFunc(
      sym::FuncKind::ConvIntString, "string", sym::FuncSig{sym::Input{intT}, stringT});
  m_funcDecls.registerFunc(
      sym::FuncKind::ConvBoolString, "string", sym::FuncSig{sym::Input{boolT}, stringT});

  // Register build-in actions.
  m_actionDecls.registerAction(sym::ActionKind::PrintString, "print", sym::Input{stringT});
}

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

auto Program::declareUserFunc(std::string name, sym::FuncSig sig) -> sym::FuncId {
  return m_funcDecls.registerFunc(sym::FuncKind::User, std::move(name), std::move(sig));
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

auto Program::updateFuncRetType(sym::FuncId funcId, sym::TypeId newRetType) -> void {
  m_funcDecls.updateFuncRetType(funcId, newRetType);
}

} // namespace prog
