#include "prog/program.hpp"
#include "internal/conversion.hpp"
#include "internal/overload.hpp"
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

Program::Program() :
    m_int{m_typeDecls.registerType(sym::TypeKind::Int, "int")},
    m_float{m_typeDecls.registerType(sym::TypeKind::Float, "float")},
    m_bool{m_typeDecls.registerType(sym::TypeKind::Bool, "bool")},
    m_string{m_typeDecls.registerType(sym::TypeKind::String, "string")} {

  using fk = typename prog::sym::FuncKind;
  using ak = typename prog::sym::ActionKind;
  using op = typename prog::Operator;

  // Register build-in unary int operators.
  m_funcDecls.registerFunc(
      *this, fk::NegateInt, getFuncName(op::Minus), sym::TypeSet{m_int}, m_int);

  // Register build-in binary int operators.
  m_funcDecls.registerFunc(
      *this, fk::AddInt, getFuncName(op::Plus), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, fk::SubInt, getFuncName(op::Minus), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, fk::MulInt, getFuncName(op::Star), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, fk::DivInt, getFuncName(op::Slash), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, fk::RemInt, getFuncName(op::Rem), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, fk::ShiftLeftInt, getFuncName(op::ShiftL), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, fk::ShiftRightInt, getFuncName(op::ShiftR), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, fk::AndInt, getFuncName(op::Amp), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, fk::OrInt, getFuncName(op::Pipe), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, fk::XorInt, getFuncName(op::Hat), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, fk::CheckEqInt, getFuncName(op::EqEq), sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckNEqInt, getFuncName(op::BangEq), sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckLeInt, getFuncName(op::Le), sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckLeEqInt, getFuncName(op::LeEq), sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckGtInt, getFuncName(op::Gt), sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckGtEqInt, getFuncName(op::GtEq), sym::TypeSet{m_int, m_int}, m_bool);

  // Register build-in unary float operators.
  m_funcDecls.registerFunc(
      *this, fk::NegateFloat, getFuncName(op::Minus), sym::TypeSet{m_float}, m_float);

  // Register build-in binary float operators.
  m_funcDecls.registerFunc(
      *this, fk::AddFloat, getFuncName(op::Plus), sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerFunc(
      *this, fk::SubFloat, getFuncName(op::Minus), sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerFunc(
      *this, fk::MulFloat, getFuncName(op::Star), sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerFunc(
      *this, fk::DivFloat, getFuncName(op::Slash), sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerFunc(
      *this, fk::CheckEqFloat, getFuncName(op::EqEq), sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckNEqFloat, getFuncName(op::BangEq), sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckLeFloat, getFuncName(op::Le), sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckLeEqFloat, getFuncName(op::LeEq), sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckGtFloat, getFuncName(op::Gt), sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckGtEqFloat, getFuncName(op::GtEq), sym::TypeSet{m_float, m_float}, m_bool);

  // Register build-in unary bool operators.
  m_funcDecls.registerFunc(*this, fk::InvBool, getFuncName(op::Bang), sym::TypeSet{m_bool}, m_bool);

  // Register build-in binary bool operators.
  m_funcDecls.registerFunc(
      *this, fk::CheckEqBool, getFuncName(op::EqEq), sym::TypeSet{m_bool, m_bool}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckNEqBool, getFuncName(op::BangEq), sym::TypeSet{m_bool, m_bool}, m_bool);

  // Register build-in binary string operators.
  m_funcDecls.registerFunc(
      *this, fk::AddString, getFuncName(op::Plus), sym::TypeSet{m_string, m_string}, m_string);
  m_funcDecls.registerFunc(
      *this, fk::CheckEqString, getFuncName(op::EqEq), sym::TypeSet{m_string, m_string}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckNEqString, getFuncName(op::BangEq), sym::TypeSet{m_string, m_string}, m_bool);

  // Register build-in default constructors.
  m_funcDecls.registerFunc(*this, fk::DefInt, "int", sym::TypeSet{}, m_int);
  m_funcDecls.registerFunc(*this, fk::DefFloat, "float", sym::TypeSet{}, m_float);
  m_funcDecls.registerFunc(*this, fk::DefBool, "bool", sym::TypeSet{}, m_bool);
  m_funcDecls.registerFunc(*this, fk::DefString, "string", sym::TypeSet{}, m_string);

  // Register build-in implicit conversions.
  m_funcDecls.registerFunc(*this, fk::ConvIntFloat, "float", sym::TypeSet{m_int}, m_float);
  m_funcDecls.registerFunc(*this, fk::ConvIntString, "string", sym::TypeSet{m_int}, m_string);
  m_funcDecls.registerFunc(*this, fk::ConvFloatString, "string", sym::TypeSet{m_float}, m_string);
  m_funcDecls.registerFunc(*this, fk::ConvBoolString, "string", sym::TypeSet{m_bool}, m_string);

  // Register build-in identity conversions (turn into no-ops).
  m_funcDecls.registerFunc(*this, fk::NoOp, "int", sym::TypeSet{m_int}, m_int);
  m_funcDecls.registerFunc(*this, fk::NoOp, "float", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(*this, fk::NoOp, "bool", sym::TypeSet{m_bool}, m_bool);
  m_funcDecls.registerFunc(*this, fk::NoOp, "string", sym::TypeSet{m_string}, m_string);

  // Register build-in explicit conversions.
  m_funcDecls.registerFunc(*this, fk::ConvFloatInt, "toInt", sym::TypeSet{m_float}, m_int);

  // Register build-in actions.
  m_actionDecls.registerAction(*this, ak::PrintString, "print", sym::TypeSet{m_string});
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

auto Program::getInt() const noexcept -> sym::TypeId { return m_int; }

auto Program::getFloat() const noexcept -> sym::TypeId { return m_float; }

auto Program::getBool() const noexcept -> sym::TypeId { return m_bool; }

auto Program::getString() const noexcept -> sym::TypeId { return m_string; }

auto Program::lookupType(const std::string& name) const -> std::optional<sym::TypeId> {
  return m_typeDecls.lookup(name);
}

auto Program::lookupFunc(const std::string& name, const sym::TypeSet& input, int maxConversions)
    const -> std::optional<sym::FuncId> {
  return m_funcDecls.lookup(*this, name, input, maxConversions);
}

auto Program::lookupFunc(
    const std::vector<sym::FuncId>& funcs, const sym::TypeSet& input, int maxConversions) const
    -> std::optional<sym::FuncId> {
  return internal::findOverload(*this, m_funcDecls, funcs, input, maxConversions);
}

auto Program::lookupFuncs(const std::string& name) const -> std::vector<sym::FuncId> {
  return m_funcDecls.lookup(name);
}

auto Program::lookupAction(const std::string& name, const sym::TypeSet& input, int maxConversions)
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

auto Program::findCommonType(const std::vector<sym::TypeId>& types) -> std::optional<sym::TypeId> {
  return internal::findCommonType(*this, types);
}

auto Program::isDelegate(sym::TypeId id) const -> bool {
  const auto& typeDecl = getTypeDecl(id);
  return typeDecl.getKind() == sym::TypeKind::UserDelegate;
}

auto Program::isCallable(sym::FuncId func, const std::vector<expr::NodePtr>& args) const -> bool {
  const auto& funcDecl = getFuncDecl(func);
  return internal::isConvertable(*this, funcDecl.getInput(), args);
}

auto Program::isCallable(sym::TypeId delegate, const std::vector<expr::NodePtr>& args) const
    -> bool {
  const auto& typeDecl = getTypeDecl(delegate);
  if (typeDecl.getKind() != sym::TypeKind::UserDelegate) {
    return false;
  }
  const auto& delegateDef = std::get<sym::DelegateDef>(getTypeDef(delegate));
  return internal::isConvertable(*this, delegateDef.getInput(), args);
}

auto Program::getTypeDecl(sym::TypeId id) const -> const sym::TypeDecl& { return m_typeDecls[id]; }

auto Program::getFuncDecl(sym::FuncId id) const -> const sym::FuncDecl& { return m_funcDecls[id]; }

auto Program::getActionDecl(sym::ActionId id) const -> const sym::ActionDecl& {
  return m_actionDecls[id];
}

auto Program::getFuncDef(sym::FuncId id) const -> const sym::FuncDef& { return m_funcDefs[id]; }

auto Program::hasTypeDef(sym::TypeId id) const -> bool { return m_typeDefs.hasDef(id); }

auto Program::getTypeDef(sym::TypeId id) const -> const sym::TypeDefTable::typeDef& {
  return m_typeDefs[id];
}

auto Program::declareUserStruct(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::UserStruct, std::move(name));
}

auto Program::declareUserUnion(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::UserUnion, std::move(name));
}

auto Program::declareUserDelegate(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::UserDelegate, std::move(name));
}

auto Program::declareUserFunc(std::string name, sym::TypeSet input, sym::TypeId output)
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
  m_funcDecls.registerFunc(*this, sym::FuncKind::MakeStruct, name, sym::TypeSet{fieldTypes}, id);

  // Register (in)equality functions.
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckEqUserType,
      getFuncName(Operator::EqEq),
      sym::TypeSet{id, id},
      m_bool);
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckNEqUserType,
      getFuncName(Operator::BangEq),
      sym::TypeSet{id, id},
      m_bool);

  // Register struct definition.
  m_typeDefs.registerStruct(m_typeDecls, id, std::move(fields));
}

auto Program::defineUserUnion(sym::TypeId id, std::vector<sym::TypeId> types) -> void {
  // Register constructor functions for each type.
  const auto& name = m_typeDecls[id].getName();
  for (const auto& type : types) {
    m_funcDecls.registerFunc(*this, sym::FuncKind::MakeUnion, name, sym::TypeSet{type}, id);
  }

  // Register (in)equality functions.
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckEqUserType,
      getFuncName(Operator::EqEq),
      sym::TypeSet{id, id},
      m_bool);
  m_funcDecls.registerFunc(
      *this,
      sym::FuncKind::CheckNEqUserType,
      getFuncName(Operator::BangEq),
      sym::TypeSet{id, id},
      m_bool);

  // Register union definition.
  m_typeDefs.registerUnion(m_typeDecls, id, std::move(types));
}

auto Program::defineUserDelegate(sym::TypeId id, sym::TypeSet input, sym::TypeId output) -> void {
  m_typeDefs.registerDelegate(m_typeDecls, id, std::move(input), output);
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
