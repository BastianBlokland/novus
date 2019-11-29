#include "prog/program.hpp"
#include "internal/conversion.hpp"
#include "prog/operator.hpp"
#include "prog/sym/action_kind.hpp"

namespace prog {

namespace internal {

auto getTypeDeclTable(const Program& prog) -> const sym::TypeDeclTable& { return prog.m_typeDecls; }

auto getFuncDeclTable(const Program& prog) -> const sym::FuncDeclTable& { return prog.m_funcDecls; }

auto getActionDeclTable(const Program& prog) -> const sym::ActionDeclTable& {
  return prog.m_actionDecls;
}

} // namespace internal

Program::Program() {
  // Register build-in types.
  const auto intT    = m_typeDecls.registerType(sym::TypeKind::Int, "int");
  const auto boolT   = m_typeDecls.registerType(sym::TypeKind::Bool, "bool");
  const auto stringT = m_typeDecls.registerType(sym::TypeKind::String, "string");

  // Register build-in unary int operators.
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::NegateInt, getFuncName(Operator::Minus), sym::Input{intT}, intT);

  // Register build-in binary int operators.
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::AddInt, getFuncName(Operator::Plus), sym::Input{intT, intT}, intT);
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::SubInt, getFuncName(Operator::Minus), sym::Input{intT, intT}, intT);
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::MulInt, getFuncName(Operator::Star), sym::Input{intT, intT}, intT);
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::DivInt, getFuncName(Operator::Slash), sym::Input{intT, intT}, intT);
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::RemInt, getFuncName(Operator::Rem), sym::Input{intT, intT}, intT);
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::CheckEqInt, getFuncName(Operator::EqEq), sym::Input{intT, intT}, boolT);
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckNEqInt,
      getFuncName(Operator::BangEq),
      sym::Input{intT, intT},
      boolT);
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::CheckLeInt, getFuncName(Operator::Le), sym::Input{intT, intT}, boolT);
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckLeEqInt,
      getFuncName(Operator::LeEq),
      sym::Input{intT, intT},
      boolT);
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::CheckGtInt, getFuncName(Operator::Gt), sym::Input{intT, intT}, boolT);
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckGtEqInt,
      getFuncName(Operator::GtEq),
      sym::Input{intT, intT},
      boolT);

  // Register build-in unary bool operators.
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::InvBool, getFuncName(Operator::Bang), sym::Input{boolT}, boolT);

  // Register build-in binary bool operators.
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckEqBool,
      getFuncName(Operator::EqEq),
      sym::Input{boolT, boolT},
      boolT);
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckNEqBool,
      getFuncName(Operator::BangEq),
      sym::Input{boolT, boolT},
      boolT);

  // Register build-in binary string operators.
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::AddString,
      getFuncName(Operator::Plus),
      sym::Input{stringT, stringT},
      stringT);
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckEqString,
      getFuncName(Operator::EqEq),
      sym::Input{stringT, stringT},
      boolT);
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckNEqString,
      getFuncName(Operator::BangEq),
      sym::Input{stringT, stringT},
      boolT);

  // Register build-in conversions.
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::ConvIntString, "string", sym::Input{intT}, stringT);
  m_funcDecls.registerFunc(
      *this, sym::FuncKind::ConvBoolString, "string", sym::Input{boolT}, stringT);

  // Register build-in actions.
  m_actionDecls.registerAction(*this, sym::ActionKind::PrintString, "print", sym::Input{stringT});
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

auto Program::lookupFunc(const std::string& name, const sym::Input& input, int maxConversions) const
    -> std::optional<sym::FuncId> {
  return m_funcDecls.lookup(*this, name, input, maxConversions);
}

auto Program::lookupFuncs(const std::string& name) const -> std::vector<sym::FuncId> {
  return m_funcDecls.lookup(name);
}

auto Program::lookupAction(const std::string& name, const sym::Input& input, int maxConversions)
    const -> std::optional<sym::ActionId> {
  return m_actionDecls.lookup(*this, name, input, maxConversions);
}

auto Program::lookupActions(const std::string& name) const -> std::vector<sym::ActionId> {
  return m_actionDecls.lookup(name);
}

auto Program::lookupConversion(sym::TypeId from, sym::TypeId to) const
    -> std::optional<sym::FuncId> {
  return internal::findConversion(*this, from, to);
}

auto Program::getTypeDecl(sym::TypeId id) const -> const sym::TypeDecl& { return m_typeDecls[id]; }

auto Program::getFuncDecl(sym::FuncId id) const -> const sym::FuncDecl& { return m_funcDecls[id]; }

auto Program::getActionDecl(sym::ActionId id) const -> const sym::ActionDecl& {
  return m_actionDecls[id];
}

auto Program::getFuncDef(sym::FuncId id) const -> const sym::FuncDef& { return m_funcDefs[id]; }

auto Program::declareUserFunc(std::string name, sym::Input input, sym::TypeId output)
    -> sym::FuncId {
  return m_funcDecls.registerFunc(
      *this, sym::FuncKind::User, std::move(name), std::move(input), output);
}

auto Program::defineUserFunc(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr)
    -> void {
  m_funcDefs.registerFunc(m_funcDecls, id, std::move(consts), std::move(expr));
}

auto Program::addExecStmt(
    sym::ActionId action, sym::ConstDeclTable consts, std::vector<expr::NodePtr> args) -> void {
  return m_execStmts.push_back(sym::execStmt(*this, action, std::move(consts), std::move(args)));
}

auto Program::updateFuncOutput(sym::FuncId funcId, sym::TypeId newOutput) -> void {
  m_funcDecls.updateFuncOutput(funcId, newOutput);
}

} // namespace prog
