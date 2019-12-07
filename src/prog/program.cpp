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
  using tk = typename sym::TypeKind;
  using fk = typename sym::FuncKind;
  using ak = typename sym::ActionKind;

  // Register build-in types.
  const auto intT    = m_typeDecls.registerType(tk::Int, "int");
  const auto floatT  = m_typeDecls.registerType(tk::Float, "float");
  const auto boolT   = m_typeDecls.registerType(tk::Bool, "bool");
  const auto stringT = m_typeDecls.registerType(tk::String, "string");

  // Register build-in unary int operators.
  m_funcDecls.registerFunc(
      *this, fk::NegateInt, getFuncName(Operator::Minus), sym::Input{intT}, intT);

  // Register build-in binary int operators.
  m_funcDecls.registerFunc(
      *this, fk::AddInt, getFuncName(Operator::Plus), sym::Input{intT, intT}, intT);
  m_funcDecls.registerFunc(
      *this, fk::SubInt, getFuncName(Operator::Minus), sym::Input{intT, intT}, intT);
  m_funcDecls.registerFunc(
      *this, fk::MulInt, getFuncName(Operator::Star), sym::Input{intT, intT}, intT);
  m_funcDecls.registerFunc(
      *this, fk::DivInt, getFuncName(Operator::Slash), sym::Input{intT, intT}, intT);
  m_funcDecls.registerFunc(
      *this, fk::RemInt, getFuncName(Operator::Rem), sym::Input{intT, intT}, intT);
  m_funcDecls.registerFunc(
      *this, fk::CheckEqInt, getFuncName(Operator::EqEq), sym::Input{intT, intT}, boolT);
  m_funcDecls.registerFunc(
      *this, fk::CheckNEqInt, getFuncName(Operator::BangEq), sym::Input{intT, intT}, boolT);
  m_funcDecls.registerFunc(
      *this, fk::CheckLeInt, getFuncName(Operator::Le), sym::Input{intT, intT}, boolT);
  m_funcDecls.registerFunc(
      *this, fk::CheckLeEqInt, getFuncName(Operator::LeEq), sym::Input{intT, intT}, boolT);
  m_funcDecls.registerFunc(
      *this, fk::CheckGtInt, getFuncName(Operator::Gt), sym::Input{intT, intT}, boolT);
  m_funcDecls.registerFunc(
      *this, fk::CheckGtEqInt, getFuncName(Operator::GtEq), sym::Input{intT, intT}, boolT);

  // Register build-in unary float operators.
  m_funcDecls.registerFunc(
      *this, fk::NegateFloat, getFuncName(Operator::Minus), sym::Input{floatT}, floatT);

  // Register build-in binary float operators.
  m_funcDecls.registerFunc(
      *this, fk::AddFloat, getFuncName(Operator::Plus), sym::Input{floatT, floatT}, floatT);
  m_funcDecls.registerFunc(
      *this, fk::SubFloat, getFuncName(Operator::Minus), sym::Input{floatT, floatT}, floatT);
  m_funcDecls.registerFunc(
      *this, fk::MulFloat, getFuncName(Operator::Star), sym::Input{floatT, floatT}, floatT);
  m_funcDecls.registerFunc(
      *this, fk::DivFloat, getFuncName(Operator::Slash), sym::Input{floatT, floatT}, floatT);
  m_funcDecls.registerFunc(
      *this, fk::CheckEqFloat, getFuncName(Operator::EqEq), sym::Input{floatT, floatT}, boolT);
  m_funcDecls.registerFunc(
      *this, fk::CheckNEqFloat, getFuncName(Operator::BangEq), sym::Input{floatT, floatT}, boolT);
  m_funcDecls.registerFunc(
      *this, fk::CheckLeFloat, getFuncName(Operator::Le), sym::Input{floatT, floatT}, boolT);
  m_funcDecls.registerFunc(
      *this, fk::CheckLeEqFloat, getFuncName(Operator::LeEq), sym::Input{floatT, floatT}, boolT);
  m_funcDecls.registerFunc(
      *this, fk::CheckGtFloat, getFuncName(Operator::Gt), sym::Input{floatT, floatT}, boolT);
  m_funcDecls.registerFunc(
      *this, fk::CheckGtEqFloat, getFuncName(Operator::GtEq), sym::Input{floatT, floatT}, boolT);

  // Register build-in unary bool operators.
  m_funcDecls.registerFunc(
      *this, fk::InvBool, getFuncName(Operator::Bang), sym::Input{boolT}, boolT);

  // Register build-in binary bool operators.
  m_funcDecls.registerFunc(
      *this, fk::CheckEqBool, getFuncName(Operator::EqEq), sym::Input{boolT, boolT}, boolT);
  m_funcDecls.registerFunc(
      *this, fk::CheckNEqBool, getFuncName(Operator::BangEq), sym::Input{boolT, boolT}, boolT);

  // Register build-in binary string operators.
  m_funcDecls.registerFunc(
      *this, fk::AddString, getFuncName(Operator::Plus), sym::Input{stringT, stringT}, stringT);
  m_funcDecls.registerFunc(
      *this, fk::CheckEqString, getFuncName(Operator::EqEq), sym::Input{stringT, stringT}, boolT);
  m_funcDecls.registerFunc(
      *this,
      fk::CheckNEqString,
      getFuncName(Operator::BangEq),
      sym::Input{stringT, stringT},
      boolT);

  // Register build-in implicit conversions.
  m_funcDecls.registerFunc(*this, fk::ConvIntFloat, "float", sym::Input{intT}, floatT);
  m_funcDecls.registerFunc(*this, fk::ConvIntString, "string", sym::Input{intT}, stringT);
  m_funcDecls.registerFunc(*this, fk::ConvFloatString, "string", sym::Input{floatT}, stringT);
  m_funcDecls.registerFunc(*this, fk::ConvBoolString, "string", sym::Input{boolT}, stringT);

  // Register build-in explicit conversions.
  m_funcDecls.registerFunc(*this, fk::ConvFloatInt, "toInt", sym::Input{floatT}, intT);

  // Register build-in actions.
  m_actionDecls.registerAction(*this, ak::PrintString, "print", sym::Input{stringT});
}

auto Program::beginTypeDecls() const -> typeDeclIterator { return m_typeDecls.begin(); }

auto Program::endTypeDecls() const -> typeDeclIterator { return m_typeDecls.end(); }

auto Program::beginFuncDecls() const -> funcDeclIterator { return m_funcDecls.begin(); }

auto Program::endFuncDecls() const -> funcDeclIterator { return m_funcDecls.end(); }

auto Program::beginActionDecls() const -> actionDeclIterator { return m_actionDecls.begin(); }

auto Program::endActionDecls() const -> actionDeclIterator { return m_actionDecls.end(); }

auto Program::beginTypeDefs() const -> typeDefIterator { return m_typeDefs.begin(); }

auto Program::endTypeDefs() const -> typeDefIterator { return m_typeDefs.end(); }

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

auto Program::getTypeDef(sym::TypeId id) const -> const sym::TypeDefTable::typeDef& {
  return m_typeDefs[id];
}

auto Program::declareUserStruct(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::UserStruct, std::move(name));
}

auto Program::declareUserUnion(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::UserUnion, std::move(name));
}

auto Program::declareUserFunc(std::string name, sym::Input input, sym::TypeId output)
    -> sym::FuncId {
  return m_funcDecls.registerFunc(
      *this, sym::FuncKind::User, std::move(name), std::move(input), output);
}

auto Program::defineUserStruct(sym::TypeId id, sym::FieldDeclTable fields) -> void {
  auto fieldTypes = std::vector<sym::TypeId>{};
  for (const auto& field : fields) {
    fieldTypes.push_back(field.getType());
  }

  // Register constructor function.
  const auto& name = m_typeDecls[id].getName();
  m_funcDecls.registerFunc(*this, sym::FuncKind::MakeStruct, name, sym::Input{fieldTypes}, id);

  // Register (in)equality functions.
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckEqUserType,
      getFuncName(Operator::EqEq),
      sym::Input{id, id},
      *m_typeDecls.lookup("bool"));
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckNEqUserType,
      getFuncName(Operator::BangEq),
      sym::Input{id, id},
      *m_typeDecls.lookup("bool"));

  // Register struct definition.
  m_typeDefs.registerStruct(m_typeDecls, id, std::move(fields));
}

auto Program::defineUserUnion(sym::TypeId id, std::vector<sym::TypeId> types) -> void {
  // Register constructor functions for each type.
  const auto& name = m_typeDecls[id].getName();
  for (const auto& type : types) {
    m_funcDecls.registerFunc(*this, sym::FuncKind::MakeUnion, name, sym::Input{type}, id);
  }

  // Register (in)equality functions.
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckEqUserType,
      getFuncName(Operator::EqEq),
      sym::Input{id, id},
      *m_typeDecls.lookup("bool"));
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckNEqUserType,
      getFuncName(Operator::BangEq),
      sym::Input{id, id},
      *m_typeDecls.lookup("bool"));

  // Register union definition.
  m_typeDefs.registerUnion(m_typeDecls, id, std::move(types));
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
