#include "prog/program.hpp"
#include "internal/implicit_conv.hpp"
#include "internal/opt_args.hpp"
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
  m_funcDecls.registerFunc(
      *this, Fk::InvLong, getFuncName(Op::Tilde), sym::TypeSet{m_long}, m_long);

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
      *this, Fk::ShiftLeftLong, getFuncName(Op::ShiftL), sym::TypeSet{m_long, m_int}, m_long);
  m_funcDecls.registerFunc(
      *this, Fk::ShiftRightLong, getFuncName(Op::ShiftR), sym::TypeSet{m_long, m_int}, m_long);
  m_funcDecls.registerFunc(
      *this, Fk::AndLong, getFuncName(Op::Amp), sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerFunc(
      *this, Fk::OrLong, getFuncName(Op::Pipe), sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerFunc(
      *this, Fk::XorLong, getFuncName(Op::Hat), sym::TypeSet{m_long, m_long}, m_long);
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
  m_funcDecls.registerIntrinsic(
      *this, Fk::PowFloat, "float_pow", sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::SqrtFloat, "float_sqrt", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::SinFloat, "float_sin", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::CosFloat, "float_cos", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::TanFloat, "float_tan", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::ASinFloat, "float_asin", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::ACosFloat, "float_acos", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::ATanFloat, "float_atan", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ATan2Float, "float_atan2", sym::TypeSet{m_float, m_float}, m_float);

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
  m_funcDecls.registerIntrinsic(
      *this, Fk::LengthString, "string_length", sym::TypeSet{m_string}, m_int);
  m_funcDecls.registerFunc(
      *this, Fk::IndexString, getFuncName(Op::SquareSquare), sym::TypeSet{m_string, m_int}, m_char);
  m_funcDecls.registerFunc(
      *this,
      Fk::SliceString,
      getFuncName(Op::SquareSquare),
      sym::TypeSet{m_string, m_int, m_int},
      m_string);

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
  m_funcDecls.registerFunc(*this, Fk::ConvLongChar, "char", sym::TypeSet{m_long}, m_char);
  m_funcDecls.registerFunc(*this, Fk::ConvFloatChar, "char", sym::TypeSet{m_float}, m_char);
  m_funcDecls.registerFunc(*this, Fk::ConvFloatLong, "long", sym::TypeSet{m_float}, m_long);
  m_funcDecls.registerFunc(*this, Fk::ConvLongFloat, "float", sym::TypeSet{m_long}, m_float);
  m_funcDecls.registerFunc(*this, Fk::ConvIntString, "string", sym::TypeSet{m_int}, m_string);
  m_funcDecls.registerFunc(*this, Fk::ConvLongString, "string", sym::TypeSet{m_long}, m_string);
  m_funcDecls.registerFunc(*this, Fk::ConvFloatString, "string", sym::TypeSet{m_float}, m_string);
  m_funcDecls.registerFunc(*this, Fk::ConvBoolString, "string", sym::TypeSet{m_bool}, m_string);
  m_funcDecls.registerFunc(*this, Fk::ConvCharString, "string", sym::TypeSet{m_char}, m_string);
  m_funcDecls.registerFunc(*this, Fk::NoOp, "asFloat", sym::TypeSet{m_int}, m_float);
  m_funcDecls.registerFunc(*this, Fk::NoOp, "asInt", sym::TypeSet{m_float}, m_int);

  // Register build-in actions.
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionEndiannessNative, "platform_endianness_native", sym::TypeSet{}, m_int);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionPlatformErrorCode, "platform_error_code", sym::TypeSet{}, m_int);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionStreamCheckValid, "stream_checkvalid", sym::TypeSet{m_stream}, m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionStreamReadString,
      "stream_read_string",
      sym::TypeSet{m_stream, m_int},
      m_string);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionStreamWriteString,
      "stream_write_string",
      sym::TypeSet{m_stream, m_string},
      m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionStreamSetOptions,
      "stream_setoptions",
      sym::TypeSet{m_stream, m_int},
      m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionStreamUnsetOptions,
      "stream_unsetoptions",
      sym::TypeSet{m_stream, m_int},
      m_bool);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionProcessStart, "process_start", sym::TypeSet{m_string}, m_process);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionProcessBlock, "process_block", sym::TypeSet{m_process}, m_int);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionProcessOpenStream,
      "process_openstream",
      sym::TypeSet{m_process, m_int},
      m_stream);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionProcessGetId, "process_getid", sym::TypeSet{m_process}, m_long);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionProcessSendSignal,
      "process_sendsignal",
      sym::TypeSet{m_process, m_int},
      m_bool);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionFileOpenStream, "file_openstream", sym::TypeSet{m_string, m_int}, m_stream);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionFileRemove, "file_remove", sym::TypeSet{m_string}, m_bool);

  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionTcpOpenCon,
      "tcp_connection_open",
      sym::TypeSet{m_string, m_int, m_int},
      m_stream);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionTcpStartServer,
      "tcp_server_start",
      sym::TypeSet{m_int, m_int, m_int},
      m_stream);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionTcpAcceptCon, "tcp_server_accept", sym::TypeSet{m_stream}, m_stream);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionTcpShutdown, "tcp_shutdown", sym::TypeSet{m_stream}, m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionIpLookupAddress,
      "ip_lookupaddress",
      sym::TypeSet{m_string, m_int},
      m_string);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionConsoleOpenStream, "console_openstream", sym::TypeSet{m_int}, m_stream);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionIsTerm, "is_term", sym::TypeSet{m_stream}, m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionTermSetOptions, "term_setoptions", sym::TypeSet{m_stream, m_int}, m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionTermUnsetOptions,
      "term_unsetoptions",
      sym::TypeSet{m_stream, m_int},
      m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionTermGetWidth, "term_getwidth", sym::TypeSet{m_stream}, m_int);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionTermGetHeight, "term_getheight", sym::TypeSet{m_stream}, m_int);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionEnvGetArg, "env_argument", sym::TypeSet{m_int}, m_string);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionEnvGetArgCount, "env_argument_count", sym::TypeSet{}, m_int);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionEnvGetVar, "env_variable", sym::TypeSet{m_string}, m_string);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionInteruptIsReq, "interupt_isrequested", sym::TypeSet{}, m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionInteruptResetReq, "interupt_reset", sym::TypeSet{}, m_bool);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionClockMicroSinceEpoch, "clock_microsinceepoch", sym::TypeSet{}, m_long);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionClockNanoSteady, "clock_nanosteady", sym::TypeSet{}, m_long);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionVersionRt, "version_runtime", sym::TypeSet{}, m_string);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionVersionCompiler, "version_compiler", sym::TypeSet{}, m_string);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionPlatformCode, "runtime_platform", sym::TypeSet{}, m_int);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionWorkingDirPath, "path_workingdirectory", sym::TypeSet{}, m_string);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionRtPath, "path_runtime", sym::TypeSet{}, m_string);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionProgramPath, "path_program", sym::TypeSet{}, m_string);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionSleepNano, "sleep_nano", sym::TypeSet{m_long}, m_bool);

  m_funcDecls.registerAction(
      *this, Fk::ActionAssert, "assert", sym::TypeSet{m_bool, m_string}, m_bool);
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

auto Program::lookupFunc(const std::string& name, OvOptions options) const
    -> std::vector<sym::FuncId> {
  return m_funcDecls.lookup(name, options);
}

auto Program::lookupIntrinsic(const std::string& name, const sym::TypeSet& input, OvOptions options)
    const -> std::optional<sym::FuncId> {
  return m_funcDecls.lookupIntrinsic(*this, name, input, options);
}

auto Program::lookupIntrinsic(const std::string& name, OvOptions options) const
    -> std::vector<sym::FuncId> {
  return m_funcDecls.lookupIntrinsic(name, options);
}

auto Program::lookupImplicitConv(sym::TypeId from, sym::TypeId to) const
    -> std::optional<sym::FuncId> {
  return internal::findImplicitConv(*this, from, to);
}

auto Program::isImplicitConvertible(sym::TypeId from, sym::TypeId to) const -> bool {
  return internal::isImplicitConvertible(*this, from, to);
}

auto Program::isImplicitConvertible(
    const sym::TypeSet& toTypes, const sym::TypeSet& fromTypes, unsigned int numOptToTypes) const
    -> bool {
  return internal::isImplicitConvertible(*this, toTypes, fromTypes, -1, numOptToTypes);
}

auto Program::findCommonType(const std::vector<sym::TypeId>& types) -> std::optional<sym::TypeId> {
  return internal::findCommonType(*this, types);
}

auto Program::isDelegate(sym::TypeId id) const -> bool {
  return id.isConcrete() && getTypeDecl(id).getKind() == sym::TypeKind::Delegate;
}

auto Program::isActionDelegate(sym::TypeId id) const -> bool {
  return isDelegate(id) && std::get<sym::DelegateDef>(getTypeDef(id)).isAction();
}

auto Program::isFuture(sym::TypeId id) const -> bool {
  return id.isConcrete() && getTypeDecl(id).getKind() == sym::TypeKind::Future;
}

auto Program::isLazy(sym::TypeId id) const -> bool {
  return id.isConcrete() && getTypeDecl(id).getKind() == sym::TypeKind::Lazy;
}

auto Program::isLazyAction(sym::TypeId id) const -> bool {
  return isLazy(id) && std::get<sym::LazyDef>(getTypeDef(id)).isAction();
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

auto Program::declarePureFunc(
    std::string name, sym::TypeSet input, sym::TypeId output, unsigned int numOptInputs)
    -> sym::FuncId {
  return m_funcDecls.registerFunc(
      *this, sym::FuncKind::User, std::move(name), std::move(input), output, numOptInputs);
}

auto Program::declareAction(
    std::string name, sym::TypeSet input, sym::TypeId output, unsigned int numOptInputs)
    -> sym::FuncId {
  return m_funcDecls.registerAction(
      *this, sym::FuncKind::User, std::move(name), std::move(input), output, numOptInputs);
}

auto Program::declareFailIntrinsic(std::string name, sym::TypeId output) -> sym::FuncId {
  return m_funcDecls.registerIntrinsicAction(
      *this, sym::FuncKind::ActionFail, std::move(name), sym::TypeSet{}, output);
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
  // Register utility intrinsics.
  m_funcDecls.registerIntrinsic(
      *this, sym::FuncKind::FutureBlock, "future_get", sym::TypeSet{id}, result);
  m_funcDecls.registerIntrinsicAction(
      *this, sym::FuncKind::FutureWaitNano, "future_waitnano", sym::TypeSet{id, m_long}, m_bool);

  // Register future definition.
  m_typeDefs.registerFuture(m_typeDecls, id, result);
}

auto Program::defineLazy(
    sym::TypeId id, sym::TypeId result, bool isAction, const std::vector<sym::TypeId>& aliases)
    -> void {
  // Register implicit conversions to the alias lazies.
  for (const auto& alias : aliases) {
    const auto& aliasDecl = getTypeDecl(alias);
    if (aliasDecl.getKind() != sym::TypeKind::Lazy) {
      throw std::invalid_argument{"Alias type has to be a lazy type"};
    }
    const auto& lazyDef = std::get<sym::LazyDef>(getTypeDef(alias));
    if (lazyDef.getResult() != result) {
      throw std::invalid_argument{"Lazy result type has to match alias lazy result type"};
    }
    m_funcDecls.registerImplicitConv(*this, sym::FuncKind::NoOp, id, alias);
  }

  // Register utility intrinsics.
  if (isAction) {
    m_funcDecls.registerIntrinsicAction(
        *this, sym::FuncKind::LazyGet, "lazy_action_get", sym::TypeSet{id}, result);
  } else {
    m_funcDecls.registerIntrinsic(
        *this, sym::FuncKind::LazyGet, "lazy_get", sym::TypeSet{id}, result);
  }

  // Register lazy definition.
  m_typeDefs.registerLazy(m_typeDecls, id, result, isAction);
}

auto Program::defineFunc(
    sym::FuncId id,
    sym::ConstDeclTable consts,
    expr::NodePtr body,
    std::vector<expr::NodePtr> optArgInitializers,
    sym::FuncDef::Flags flags) -> void {
  m_funcDefs.registerFunc(
      m_funcDecls, id, std::move(consts), std::move(body), std::move(optArgInitializers), flags);
}

auto Program::addExecStmt(sym::ConstDeclTable consts, expr::NodePtr expr) -> void {
  return m_execStmts.push_back(sym::execStmt(std::move(consts), std::move(expr)));
}

auto Program::updateFuncOutput(sym::FuncId funcId, sym::TypeId newOutput) -> void {
  m_funcDecls.updateFuncOutput(funcId, newOutput);
}

auto Program::applyOptCallArgs() -> void { internal::applyOptArgIntializers(*this); }

} // namespace prog
