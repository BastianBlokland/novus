#include "prog/program.hpp"
#include "internal/implicit_conv.hpp"
#include "internal/overload.hpp"
#include "prog/operator.hpp"

namespace prog {

namespace internal {

auto getTypeDeclTable(const Program& prog) -> const sym::TypeDeclTable& { return prog.m_typeDecls; }

auto getFuncDeclTable(const Program& prog) -> const sym::FuncDeclTable& { return prog.m_funcDecls; }

auto getTypeDeclTable(Program* prog) -> sym::TypeDeclTable& { return prog->m_typeDecls; }

auto getFuncDeclTable(Program* prog) -> sym::FuncDeclTable& { return prog->m_funcDecls; }

auto getTypeDefTable(Program* prog) -> sym::TypeDefTable& { return prog->m_typeDefs; }

auto getFuncDefTable(Program* prog) -> sym::FuncDefTable& { return prog->m_funcDefs; }

} // namespace internal

Program::Program() :
    m_int{m_typeDecls.registerType(sym::TypeKind::Int, "int")},
    m_long{m_typeDecls.registerType(sym::TypeKind::Long, "long")},
    m_float{m_typeDecls.registerType(sym::TypeKind::Float, "float")},
    m_bool{m_typeDecls.registerType(sym::TypeKind::Bool, "bool")},
    m_string{m_typeDecls.registerType(sym::TypeKind::String, "string")},
    m_char{m_typeDecls.registerType(sym::TypeKind::Char, "char")},
    m_stream{m_typeDecls.registerType(sym::TypeKind::Stream, "sys_stream")},
    m_process{m_typeDecls.registerType(sym::TypeKind::Process, "sys_process")} {

  using Fk = prog::sym::FuncKind;
  using Op = prog::Operator;

  // Register build-in unary int operators.
  m_funcDecls.registerFunc(
      *this, Fk::NegateInt, getFuncName(Op::Minus), sym::TypeSet{m_int}, m_int);
  m_funcDecls.registerFunc(*this, Fk::NoOp, getFuncName(Op::Plus), sym::TypeSet{m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::IncrementInt, getFuncName(Op::PlusPlus), sym::TypeSet{m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::DecrementInt, getFuncName(Op::MinusMinus), sym::TypeSet{m_int}, m_int);
  m_funcDecls.registerFunc(*this, Fk::InvInt, getFuncName(Op::Tilde), sym::TypeSet{m_int}, m_int);

  // Register build-in binary int operators.
  m_funcDecls.registerFunc(
      *this, Fk::AddInt, getFuncName(Op::Plus), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::SubInt, getFuncName(Op::Minus), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::MulInt, getFuncName(Op::Star), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::DivInt, getFuncName(Op::Slash), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::RemInt, getFuncName(Op::Rem), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::ShiftLeftInt, getFuncName(Op::ShiftL), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::ShiftRightInt, getFuncName(Op::ShiftR), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::AndInt, getFuncName(Op::Amp), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::OrInt, getFuncName(Op::Pipe), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::XorInt, getFuncName(Op::Hat), sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::CheckEqInt, getFuncName(Op::EqEq), sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckNEqInt, getFuncName(Op::BangEq), sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckLeInt, getFuncName(Op::Le), sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckLeEqInt, getFuncName(Op::LeEq), sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckGtInt, getFuncName(Op::Gt), sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckGtEqInt, getFuncName(Op::GtEq), sym::TypeSet{m_int, m_int}, m_bool);

  // Register build-in unary long operators.
  m_funcDecls.registerFunc(
      *this, Fk::NegateLong, getFuncName(Op::Minus), sym::TypeSet{m_long}, m_long);
  m_funcDecls.registerFunc(*this, Fk::NoOp, getFuncName(Op::Plus), sym::TypeSet{m_long}, m_long);
  m_funcDecls.registerFunc(
      *this, Fk::IncrementLong, getFuncName(Op::PlusPlus), sym::TypeSet{m_long}, m_long);
  m_funcDecls.registerFunc(
      *this, Fk::DecrementLong, getFuncName(Op::MinusMinus), sym::TypeSet{m_long}, m_long);

  // Register build-in binary long operators.
  m_funcDecls.registerFunc(
      *this, Fk::AddLong, getFuncName(Op::Plus), sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerFunc(
      *this, Fk::SubLong, getFuncName(Op::Minus), sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerFunc(
      *this, Fk::MulLong, getFuncName(Op::Star), sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerFunc(
      *this, Fk::DivLong, getFuncName(Op::Slash), sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerFunc(
      *this, Fk::RemLong, getFuncName(Op::Rem), sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerFunc(
      *this, Fk::CheckEqLong, getFuncName(Op::EqEq), sym::TypeSet{m_long, m_long}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckNEqLong, getFuncName(Op::BangEq), sym::TypeSet{m_long, m_long}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckLeLong, getFuncName(Op::Le), sym::TypeSet{m_long, m_long}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckLeEqLong, getFuncName(Op::LeEq), sym::TypeSet{m_long, m_long}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckGtLong, getFuncName(Op::Gt), sym::TypeSet{m_long, m_long}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckGtEqLong, getFuncName(Op::GtEq), sym::TypeSet{m_long, m_long}, m_bool);

  // Register build-in unary float operators.
  m_funcDecls.registerFunc(
      *this, Fk::NegateFloat, getFuncName(Op::Minus), sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(*this, Fk::NoOp, getFuncName(Op::Plus), sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(
      *this, Fk::IncrementFloat, getFuncName(Op::PlusPlus), sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(
      *this, Fk::DecrementFloat, getFuncName(Op::MinusMinus), sym::TypeSet{m_float}, m_float);

  // Register build-in binary float operators.
  m_funcDecls.registerFunc(
      *this, Fk::AddFloat, getFuncName(Op::Plus), sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerFunc(
      *this, Fk::SubFloat, getFuncName(Op::Minus), sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerFunc(
      *this, Fk::MulFloat, getFuncName(Op::Star), sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerFunc(
      *this, Fk::DivFloat, getFuncName(Op::Slash), sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerFunc(
      *this, Fk::ModFloat, getFuncName(Op::Rem), sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerFunc(
      *this, Fk::CheckEqFloat, getFuncName(Op::EqEq), sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckNEqFloat, getFuncName(Op::BangEq), sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckLeFloat, getFuncName(Op::Le), sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckLeEqFloat, getFuncName(Op::LeEq), sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckGtFloat, getFuncName(Op::Gt), sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckGtEqFloat, getFuncName(Op::GtEq), sym::TypeSet{m_float, m_float}, m_bool);

  // Register build-in float functions.
  m_funcDecls.registerFunc(*this, Fk::PowFloat, "pow", sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerFunc(*this, Fk::SqrtFloat, "sqrt", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(*this, Fk::SinFloat, "sin", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(*this, Fk::CosFloat, "cos", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(*this, Fk::TanFloat, "tan", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(*this, Fk::ASinFloat, "asin", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(*this, Fk::ACosFloat, "acos", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(*this, Fk::ATanFloat, "atan", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(*this, Fk::ATan2Float, "atan2", sym::TypeSet{m_float, m_float}, m_float);

  // Register build-in unary bool operators.
  m_funcDecls.registerFunc(*this, Fk::InvBool, getFuncName(Op::Bang), sym::TypeSet{m_bool}, m_bool);

  // Register build-in binary bool operators.
  m_funcDecls.registerFunc(
      *this, Fk::CheckEqBool, getFuncName(Op::EqEq), sym::TypeSet{m_bool, m_bool}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckNEqBool, getFuncName(Op::BangEq), sym::TypeSet{m_bool, m_bool}, m_bool);

  // Register build-in binary string operators.
  m_funcDecls.registerFunc(
      *this, Fk::AddString, getFuncName(Op::Plus), sym::TypeSet{m_string, m_string}, m_string);
  m_funcDecls.registerFunc(
      *this, Fk::AppendChar, getFuncName(Op::Plus), sym::TypeSet{m_string, m_char}, m_string);
  m_funcDecls.registerFunc(
      *this, Fk::CheckEqString, getFuncName(Op::EqEq), sym::TypeSet{m_string, m_string}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckNEqString, getFuncName(Op::BangEq), sym::TypeSet{m_string, m_string}, m_bool);

  // Register build-in string functions.
  m_funcDecls.registerFunc(*this, Fk::LengthString, "length", sym::TypeSet{m_string}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::IndexString, getFuncName(Op::SquareSquare), sym::TypeSet{m_string, m_int}, m_char);
  m_funcDecls.registerFunc(
      *this,
      Fk::SliceString,
      getFuncName(Op::SquareSquare),
      sym::TypeSet{m_string, m_int, m_int},
      m_string);

  // Register build-in unary char operators.
  m_funcDecls.registerFunc(
      *this, Fk::IncrementChar, getFuncName(Op::PlusPlus), sym::TypeSet{m_char}, m_char);
  m_funcDecls.registerFunc(
      *this, Fk::DecrementChar, getFuncName(Op::MinusMinus), sym::TypeSet{m_char}, m_char);

  // Register build-in binary char operators.
  m_funcDecls.registerFunc(
      *this, Fk::CombineChar, getFuncName(Op::Plus), sym::TypeSet{m_char, m_char}, m_string);
  m_funcDecls.registerFunc(
      *this, Fk::CheckEqInt, getFuncName(Op::EqEq), sym::TypeSet{m_char, m_char}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckNEqInt, getFuncName(Op::BangEq), sym::TypeSet{m_char, m_char}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckLeInt, getFuncName(Op::Le), sym::TypeSet{m_char, m_char}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckLeEqInt, getFuncName(Op::LeEq), sym::TypeSet{m_char, m_char}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckGtInt, getFuncName(Op::Gt), sym::TypeSet{m_char, m_char}, m_bool);
  m_funcDecls.registerFunc(
      *this, Fk::CheckGtEqInt, getFuncName(Op::GtEq), sym::TypeSet{m_char, m_char}, m_bool);

  // Register build-in default constructors.
  m_funcDecls.registerFunc(*this, Fk::DefInt, "int", sym::TypeSet{}, m_int);
  m_funcDecls.registerFunc(*this, Fk::DefLong, "long", sym::TypeSet{}, m_long);
  m_funcDecls.registerFunc(*this, Fk::DefFloat, "float", sym::TypeSet{}, m_float);
  m_funcDecls.registerFunc(*this, Fk::DefBool, "bool", sym::TypeSet{}, m_bool);
  m_funcDecls.registerFunc(*this, Fk::DefString, "string", sym::TypeSet{}, m_string);

  // Register build-in implicit conversions.
  m_funcDecls.registerImplicitConv(*this, Fk::NoOp, m_char, m_int);
  m_funcDecls.registerImplicitConv(*this, Fk::ConvIntLong, m_int, m_long);
  m_funcDecls.registerImplicitConv(*this, Fk::ConvCharLong, m_char, m_long);
  m_funcDecls.registerImplicitConv(*this, Fk::ConvIntFloat, m_int, m_float);
  m_funcDecls.registerImplicitConv(*this, Fk::ConvLongFloat, m_long, m_float);

  // Register build-in identity conversions (turns into no-ops).
  m_funcDecls.registerFunc(*this, Fk::NoOp, "int", sym::TypeSet{m_int}, m_int);
  m_funcDecls.registerFunc(*this, Fk::NoOp, "long", sym::TypeSet{m_long}, m_long);
  m_funcDecls.registerFunc(*this, Fk::NoOp, "float", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerFunc(*this, Fk::NoOp, "bool", sym::TypeSet{m_bool}, m_bool);
  m_funcDecls.registerFunc(*this, Fk::NoOp, "string", sym::TypeSet{m_string}, m_string);
  m_funcDecls.registerFunc(*this, Fk::NoOp, "char", sym::TypeSet{m_char}, m_char);

  // Register build-in explicit conversions.
  m_funcDecls.registerFunc(*this, Fk::ConvFloatInt, "int", sym::TypeSet{m_float}, m_int);
  m_funcDecls.registerFunc(*this, Fk::ConvLongInt, "int", sym::TypeSet{m_long}, m_int);
  m_funcDecls.registerFunc(*this, Fk::ConvIntChar, "char", sym::TypeSet{m_int}, m_char);
  m_funcDecls.registerFunc(*this, Fk::ConvFloatChar, "char", sym::TypeSet{m_float}, m_char);
  m_funcDecls.registerFunc(*this, Fk::ConvFloatLong, "long", sym::TypeSet{m_float}, m_long);
  m_funcDecls.registerFunc(*this, Fk::ConvIntString, "string", sym::TypeSet{m_int}, m_string);
  m_funcDecls.registerFunc(*this, Fk::ConvLongString, "string", sym::TypeSet{m_long}, m_string);
  m_funcDecls.registerFunc(*this, Fk::ConvFloatString, "string", sym::TypeSet{m_float}, m_string);
  m_funcDecls.registerFunc(*this, Fk::ConvBoolString, "string", sym::TypeSet{m_bool}, m_string);
  m_funcDecls.registerFunc(*this, Fk::ConvCharString, "string", sym::TypeSet{m_char}, m_string);
  m_funcDecls.registerFunc(*this, Fk::NoOp, "asFloat", sym::TypeSet{m_int}, m_float);
  m_funcDecls.registerFunc(*this, Fk::NoOp, "asInt", sym::TypeSet{m_float}, m_int);

  // Register build-in actions.
  m_funcDecls.registerAction(
      *this, Fk::ActionStreamCheckValid, "streamCheckValid", sym::TypeSet{m_stream}, m_bool);
  m_funcDecls.registerAction(
      *this, Fk::ActionStreamReadString, "streamRead", sym::TypeSet{m_stream, m_int}, m_string);
  m_funcDecls.registerAction(
      *this, Fk::ActionStreamReadChar, "streamRead", sym::TypeSet{m_stream}, m_char);
  m_funcDecls.registerAction(
      *this, Fk::ActionStreamWriteString, "streamWrite", sym::TypeSet{m_stream, m_string}, m_bool);
  m_funcDecls.registerAction(
      *this, Fk::ActionStreamWriteChar, "streamWrite", sym::TypeSet{m_stream, m_char}, m_bool);
  m_funcDecls.registerAction(
      *this, Fk::ActionStreamFlush, "streamFlush", sym::TypeSet{m_stream}, m_bool);
  m_funcDecls.registerAction(
      *this, Fk::ActionStreamSetOptions, "streamSetOptions", sym::TypeSet{m_stream, m_int}, m_bool);
  m_funcDecls.registerAction(
      *this,
      Fk::ActionStreamUnsetOptions,
      "streamUnsetOptions",
      sym::TypeSet{m_stream, m_int},
      m_bool);

  m_funcDecls.registerAction(
      *this, Fk::ActionProcessStart, "processStart", sym::TypeSet{m_string}, m_process);
  m_funcDecls.registerAction(
      *this, Fk::ActionProcessBlock, "processBlock", sym::TypeSet{m_process}, m_int);
  m_funcDecls.registerAction(
      *this,
      Fk::ActionProcessOpenStream,
      "processOpenStream",
      sym::TypeSet{m_process, m_int},
      m_stream);
  m_funcDecls.registerAction(
      *this, Fk::ActionProcessGetId, "processGetId", sym::TypeSet{m_process}, m_long);
  m_funcDecls.registerAction(
      *this,
      Fk::ActionProcessSendSignal,
      "processSendSignal",
      sym::TypeSet{m_process, m_int},
      m_bool);

  m_funcDecls.registerAction(
      *this, Fk::ActionFileOpenStream, "fileOpenStream", sym::TypeSet{m_string, m_int}, m_stream);
  m_funcDecls.registerAction(
      *this, Fk::ActionFileRemove, "fileRemove", sym::TypeSet{m_string}, m_bool);

  m_funcDecls.registerAction(
      *this, Fk::ActionConsoleOpenStream, "consoleOpenStream", sym::TypeSet{m_int}, m_stream);

  m_funcDecls.registerAction(
      *this, Fk::ActionTcpOpenCon, "tcpOpenConnection", sym::TypeSet{m_string, m_int}, m_stream);
  m_funcDecls.registerAction(
      *this, Fk::ActionTcpStartServer, "tcpStartServer", sym::TypeSet{m_int, m_int}, m_stream);
  m_funcDecls.registerAction(
      *this, Fk::ActionTcpAcceptCon, "tcpAcceptConnection", sym::TypeSet{m_stream}, m_stream);
  m_funcDecls.registerAction(
      *this, Fk::ActionIpLookupAddress, "ipLookupAddress", sym::TypeSet{m_string}, m_string);

  m_funcDecls.registerAction(
      *this, Fk::ActionTermSetOptions, "termSetOptions", sym::TypeSet{m_int}, m_bool);
  m_funcDecls.registerAction(
      *this, Fk::ActionTermUnsetOptions, "termUnsetOptions", sym::TypeSet{m_int}, m_bool);
  m_funcDecls.registerAction(*this, Fk::ActionTermGetWidth, "termGetWidth", sym::TypeSet{}, m_int);
  m_funcDecls.registerAction(
      *this, Fk::ActionTermGetHeight, "termGetHeight", sym::TypeSet{}, m_int);

  m_funcDecls.registerAction(
      *this, Fk::ActionEnvGetArg, "envGetArg", sym::TypeSet{m_int}, m_string);
  m_funcDecls.registerAction(
      *this, Fk::ActionEnvGetArgCount, "envGetArgCount", sym::TypeSet{}, m_int);
  m_funcDecls.registerAction(
      *this, Fk::ActionEnvGetVar, "envGetVar", sym::TypeSet{m_string}, m_string);
  m_funcDecls.registerAction(
      *this, Fk::ActionInteruptIsReq, "interuptIsRequested", sym::TypeSet{}, m_bool);
  m_funcDecls.registerAction(
      *this, Fk::ActionInteruptResetReq, "interuptResetRequested", sym::TypeSet{}, m_bool);

  m_funcDecls.registerAction(
      *this, Fk::ActionClockMicroSinceEpoch, "clockMicroSinceEpoch", sym::TypeSet{}, m_long);
  m_funcDecls.registerAction(
      *this, Fk::ActionClockNanoSteady, "clockNanoSteady", sym::TypeSet{}, m_long);

  m_funcDecls.registerAction(
      *this, Fk::ActionVersionRt, "runtimeVersionString", sym::TypeSet{}, m_string);
  m_funcDecls.registerAction(
      *this, Fk::ActionVersionCompiler, "compilerVersionString", sym::TypeSet{}, m_string);

  m_funcDecls.registerAction(*this, Fk::ActionPlatformCode, "platformCode", sym::TypeSet{}, m_int);
  m_funcDecls.registerAction(
      *this, Fk::ActionWorkingDirPath, "workingDirectoryPathString", sym::TypeSet{}, m_string);
  m_funcDecls.registerAction(
      *this, Fk::ActionProgramPath, "programPathString", sym::TypeSet{}, m_string);

  m_funcDecls.registerAction(*this, Fk::ActionSleepNano, "sleepNano", sym::TypeSet{m_long}, m_long);
  m_funcDecls.registerAction(
      *this, Fk::ActionAssert, "assert", sym::TypeSet{m_bool, m_string}, m_bool);
  m_funcDecls.registerAction(*this, Fk::ActionFail, "fail", sym::TypeSet{}, m_bool);
}

auto Program::hasType(const std::string& name) const -> bool { return m_typeDecls.exists(name); }

auto Program::lookupType(const std::string& name) const -> std::optional<sym::TypeId> {
  return m_typeDecls.lookup(name);
}

auto Program::hasFunc(const std::string& name) const -> bool { return m_funcDecls.exists(name); }

auto Program::lookupFunc(const std::string& name, const sym::TypeSet& input, OvOptions options)
    const -> std::optional<sym::FuncId> {
  return m_funcDecls.lookup(*this, name, input, options);
}

auto Program::lookupFunc(
    const std::vector<sym::FuncId>& funcs, const sym::TypeSet& input, OvOptions options) const
    -> std::optional<sym::FuncId> {
  return internal::findOverload(*this, m_funcDecls, funcs, input, options);
}

auto Program::lookupFuncs(const std::string& name, OvOptions options) const
    -> std::vector<sym::FuncId> {
  return m_funcDecls.lookup(name, options);
}

auto Program::lookupImplicitConv(sym::TypeId from, sym::TypeId to) const
    -> std::optional<sym::FuncId> {
  return internal::findImplicitConv(*this, from, to);
}

auto Program::isImplicitConvertible(sym::TypeId from, sym::TypeId to) const -> bool {
  return internal::isImplicitConvertible(*this, from, to);
}

auto Program::isImplicitConvertible(
    const sym::TypeSet& toTypes, const sym::TypeSet& fromTypes) const -> bool {
  return internal::isImplicitConvertible(*this, toTypes, fromTypes);
}

auto Program::findCommonType(const std::vector<sym::TypeId>& types) -> std::optional<sym::TypeId> {
  return internal::findCommonType(*this, types);
}

auto Program::isDelegate(sym::TypeId id) const -> bool {
  if (!id.isConcrete()) {
    return false;
  }
  const auto& typeDecl = getTypeDecl(id);
  return typeDecl.getKind() == sym::TypeKind::Delegate;
}

auto Program::isActionDelegate(sym::TypeId id) const -> bool {
  if (!isDelegate(id)) {
    return false;
  }
  const auto& delegateDef = std::get<sym::DelegateDef>(getTypeDef(id));
  return delegateDef.isAction();
}

auto Program::isFuture(sym::TypeId id) const -> bool {
  if (!id.isConcrete()) {
    return false;
  }
  const auto& typeDecl = getTypeDecl(id);
  return typeDecl.getKind() == sym::TypeKind::Future;
}

auto Program::isLazy(sym::TypeId id) const -> bool {
  if (!id.isConcrete()) {
    return false;
  }
  const auto& typeDecl = getTypeDecl(id);
  return typeDecl.getKind() == sym::TypeKind::Lazy;
}

auto Program::satisfiesOptions(sym::TypeId delegate, OvOptions options) const -> bool {
  if (!delegate.isConcrete()) {
    return false;
  }
  const auto& typeDecl = getTypeDecl(delegate);
  if (typeDecl.getKind() != sym::TypeKind::Delegate) {
    return false;
  }
  const auto& delegateDef = std::get<sym::DelegateDef>(getTypeDef(delegate));
  if (options.hasFlag<OvFlags::ExclActions>() && delegateDef.isAction()) {
    return false;
  }
  if (options.hasFlag<OvFlags::ExclPureFuncs>() && !delegateDef.isAction()) {
    return false;
  }
  return true;
}

auto Program::isCallable(sym::TypeId delegate, const sym::TypeSet& input, OvOptions options) const
    -> bool {

  if (!delegate.isConcrete()) {
    return false;
  }
  const auto& typeDecl = getTypeDecl(delegate);
  if (typeDecl.getKind() != sym::TypeKind::Delegate) {
    return false;
  }
  const auto& delegateDef = std::get<sym::DelegateDef>(getTypeDef(delegate));
  if (options.hasFlag<OvFlags::ExclActions>() && delegateDef.isAction()) {
    return false;
  }
  if (options.hasFlag<OvFlags::ExclPureFuncs>() && !delegateDef.isAction()) {
    return false;
  }
  return internal::isImplicitConvertible(
      *this, delegateDef.getInput(), input, options.getMaxImplicitConvs());
}

auto Program::getDelegateRetType(sym::TypeId id) const -> std::optional<sym::TypeId> {
  if (!id.isConcrete()) {
    return std::nullopt;
  }
  const auto& typeDecl = getTypeDecl(id);
  if (typeDecl.getKind() != sym::TypeKind::Delegate) {
    return std::nullopt;
  }
  return std::get<sym::DelegateDef>(getTypeDef(id)).getOutput();
}

auto Program::getTypeDecl(sym::TypeId id) const -> const sym::TypeDecl& { return m_typeDecls[id]; }

auto Program::getFuncDecl(sym::FuncId id) const -> const sym::FuncDecl& { return m_funcDecls[id]; }

auto Program::getFuncDef(sym::FuncId id) const -> const sym::FuncDef& { return m_funcDefs[id]; }

auto Program::hasTypeDef(sym::TypeId id) const -> bool { return m_typeDefs.hasDef(id); }

auto Program::getTypeDef(sym::TypeId id) const -> const sym::TypeDefTable::TypeDef& {
  return m_typeDefs[id];
}

auto Program::declareStruct(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::Struct, std::move(name));
}

auto Program::declareUnion(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::Union, std::move(name));
}

auto Program::declareEnum(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::Enum, std::move(name));
}

auto Program::declareDelegate(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::Delegate, std::move(name));
}

auto Program::declareFuture(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::Future, std::move(name));
}

auto Program::declareLazy(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::Lazy, std::move(name));
}

auto Program::declarePureFunc(std::string name, sym::TypeSet input, sym::TypeId output)
    -> sym::FuncId {
  return m_funcDecls.registerFunc(
      *this, sym::FuncKind::User, std::move(name), std::move(input), output);
}

auto Program::declareAction(std::string name, sym::TypeSet input, sym::TypeId output)
    -> sym::FuncId {
  return m_funcDecls.registerAction(
      *this, sym::FuncKind::User, std::move(name), std::move(input), output);
}

auto Program::defineStruct(sym::TypeId id, sym::FieldDeclTable fields) -> void {
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

auto Program::defineUnion(sym::TypeId id, std::vector<sym::TypeId> types) -> void {

  // Register constructor functions for each type.
  for (const auto& type : types) {
    m_funcDecls.registerImplicitConv(*this, sym::FuncKind::MakeUnion, type, id);
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

auto Program::defineEnum(sym::TypeId id, std::unordered_map<std::string, int32_t> entries) -> void {
  using fk = sym::FuncKind;

  const auto& name = m_typeDecls[id].getName();

  // Register explicit conversion from int.
  m_funcDecls.registerFunc(*this, fk::NoOp, name, sym::TypeSet{m_int}, id);

  // Register implicit conversion to int and long.
  m_funcDecls.registerImplicitConv(*this, fk::NoOp, id, m_int);
  m_funcDecls.registerImplicitConv(*this, fk::ConvIntLong, id, m_long);

  // Register ordering operators (<, <=, >, >=).
  m_funcDecls.registerFunc(
      *this, fk::CheckLeInt, getFuncName(Operator::Le), sym::TypeSet{id, id}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckLeEqInt, getFuncName(Operator::LeEq), sym::TypeSet{id, id}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckGtInt, getFuncName(Operator::Gt), sym::TypeSet{id, id}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckGtEqInt, getFuncName(Operator::GtEq), sym::TypeSet{id, id}, m_bool);

  // Register bitwise & and | operators.
  m_funcDecls.registerFunc(*this, fk::OrInt, getFuncName(Operator::Pipe), sym::TypeSet{id, id}, id);
  m_funcDecls.registerFunc(*this, fk::AndInt, getFuncName(Operator::Amp), sym::TypeSet{id, id}, id);

  // Register (in)equality functions.
  m_funcDecls.registerFunc(
      *this, fk::CheckEqInt, getFuncName(Operator::EqEq), sym::TypeSet{id, id}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckNEqInt, getFuncName(Operator::BangEq), sym::TypeSet{id, id}, m_bool);

  // Register enum definition.
  m_typeDefs.registerEnum(m_typeDecls, id, std::move(entries));
}

auto Program::defineDelegate(
    sym::TypeId id,
    bool isAction,
    sym::TypeSet input,
    sym::TypeId output,
    const std::vector<sym::TypeId>& aliases) -> void {

  // Register implicit conversions to the alias delegates.
  for (const auto& alias : aliases) {
    const auto& aliasDecl = getTypeDecl(alias);
    if (aliasDecl.getKind() != sym::TypeKind::Delegate) {
      throw std::invalid_argument{"Alias type has to be a delegate"};
    }
    const auto& delegateDef = std::get<sym::DelegateDef>(getTypeDef(alias));
    if (input != delegateDef.getInput() || output != delegateDef.getOutput()) {
      throw std::invalid_argument{"Delegate signature has to match alias delegate signature"};
    }
    m_funcDecls.registerImplicitConv(*this, sym::FuncKind::NoOp, id, alias);
  }

  // Register delegate definition.
  m_typeDefs.registerDelegate(m_typeDecls, id, isAction, std::move(input), output);
}

auto Program::defineFuture(sym::TypeId id, sym::TypeId result) -> void {
  // Register utility functions and actions.
  m_funcDecls.registerAction(
      *this, sym::FuncKind::FutureWaitNano, "waitNano", sym::TypeSet{id, m_long}, m_bool);

  m_funcDecls.registerFunc(*this, sym::FuncKind::FutureBlock, "get", sym::TypeSet{id}, result);

  // Register future definition.
  m_typeDefs.registerFuture(m_typeDecls, id, result);
}

auto Program::defineLazy(sym::TypeId id, sym::TypeId result) -> void {
  // Register utility functions.
  m_funcDecls.registerFunc(*this, sym::FuncKind::LazyGet, "get", sym::TypeSet{id}, result);

  // Register lazy definition.
  m_typeDefs.registerLazy(m_typeDecls, id, result);
}

auto Program::defineFunc(sym::FuncId id, sym::ConstDeclTable consts, expr::NodePtr expr) -> void {
  m_funcDefs.registerFunc(m_funcDecls, id, std::move(consts), std::move(expr));
}

auto Program::addExecStmt(sym::ConstDeclTable consts, expr::NodePtr expr) -> void {
  return m_execStmts.push_back(sym::execStmt(std::move(consts), std::move(expr)));
}

auto Program::updateFuncOutput(sym::FuncId funcId, sym::TypeId newOutput) -> void {
  m_funcDecls.updateFuncOutput(funcId, newOutput);
}

} // namespace prog
