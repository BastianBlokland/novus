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
    m_char{m_typeDecls.registerType(sym::TypeKind::Char, "char")},
    m_string{m_typeDecls.registerType(sym::TypeKind::String, "string")},
    m_sysStream{m_typeDecls.registerType(sym::TypeKind::SysStream, "sys_stream")},
    m_sysProcess{m_typeDecls.registerType(sym::TypeKind::SysProcess, "sys_process")} {

  using Fk = prog::sym::FuncKind;

  // Register int intrinsics.
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckEqInt, "int_eq_int", sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckIntZero, "int_eq_zero", sym::TypeSet{m_int}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckLeInt, "int_le_int", sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckGtInt, "int_gt_int", sym::TypeSet{m_int, m_int}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::AddInt, "int_add_int", sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerIntrinsic(
      *this, Fk::SubInt, "int_sub_int", sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerIntrinsic(*this, Fk::NegateInt, "int_neg", sym::TypeSet{m_int}, m_int);
  m_funcDecls.registerIntrinsic(*this, Fk::InvInt, "int_inv", sym::TypeSet{m_int}, m_int);
  m_funcDecls.registerIntrinsic(
      *this, Fk::MulInt, "int_mul_int", sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerIntrinsic(
      *this, Fk::DivInt, "int_div_int", sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerIntrinsic(
      *this, Fk::RemInt, "int_rem_int", sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ShiftLeftInt, "int_shiftleft", sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ShiftRightInt, "int_shiftright", sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerIntrinsic(
      *this, Fk::AndInt, "int_and_int", sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerIntrinsic(*this, Fk::OrInt, "int_or_int", sym::TypeSet{m_int, m_int}, m_int);
  m_funcDecls.registerIntrinsic(
      *this, Fk::XorInt, "int_xor_int", sym::TypeSet{m_int, m_int}, m_int);

  // Register long intrinsics.
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckEqLong, "long_eq_long", sym::TypeSet{m_long, m_long}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckLeLong, "long_le_long", sym::TypeSet{m_long, m_long}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckGtLong, "long_gt_long", sym::TypeSet{m_long, m_long}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::AddLong, "long_add_long", sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerIntrinsic(
      *this, Fk::SubLong, "long_sub_long", sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerIntrinsic(*this, Fk::NegateLong, "long_neg", sym::TypeSet{m_long}, m_long);
  m_funcDecls.registerIntrinsic(*this, Fk::InvLong, "long_inv", sym::TypeSet{m_long}, m_long);
  m_funcDecls.registerIntrinsic(
      *this, Fk::MulLong, "long_mul_long", sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerIntrinsic(
      *this, Fk::DivLong, "long_div_long", sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerIntrinsic(
      *this, Fk::RemLong, "long_rem_long", sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ShiftLeftLong, "long_shiftleft", sym::TypeSet{m_long, m_int}, m_long);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ShiftRightLong, "long_shiftright", sym::TypeSet{m_long, m_int}, m_long);
  m_funcDecls.registerIntrinsic(
      *this, Fk::AndLong, "long_and_long", sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerIntrinsic(
      *this, Fk::OrLong, "long_or_long", sym::TypeSet{m_long, m_long}, m_long);
  m_funcDecls.registerIntrinsic(
      *this, Fk::XorLong, "long_xor_long", sym::TypeSet{m_long, m_long}, m_long);

  // Register float intrinsics.
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckEqFloat, "float_eq_float", sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckLeFloat, "float_le_float", sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckGtFloat, "float_gt_float", sym::TypeSet{m_float, m_float}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::AddFloat, "float_add_float", sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerIntrinsic(
      *this, Fk::SubFloat, "float_sub_float", sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerIntrinsic(
      *this, Fk::NegateFloat, "float_neg", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(
      *this, Fk::PowFloat, "float_pow", sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerIntrinsic(
      *this, Fk::MulFloat, "float_mul_float", sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerIntrinsic(
      *this, Fk::DivFloat, "float_div_float", sym::TypeSet{m_float, m_float}, m_float);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ModFloat, "float_mod_float", sym::TypeSet{m_float, m_float}, m_float);

  m_funcDecls.registerIntrinsic(*this, Fk::SqrtFloat, "float_sqrt", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::SinFloat, "float_sin", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::CosFloat, "float_cos", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::TanFloat, "float_tan", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::ASinFloat, "float_asin", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::ACosFloat, "float_acos", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::ATanFloat, "float_atan", sym::TypeSet{m_float}, m_float);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ATan2Float, "float_atan2", sym::TypeSet{m_float, m_float}, m_float);

  // Register string intrinsics.
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckEqString, "string_eq_string", sym::TypeSet{m_string, m_string}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::CheckStringEmpty, "string_eq_empty", sym::TypeSet{m_string}, m_bool);
  m_funcDecls.registerIntrinsic(
      *this, Fk::AddString, "string_add_string", sym::TypeSet{m_string, m_string}, m_string);
  m_funcDecls.registerIntrinsic(
      *this, Fk::AppendChar, "string_add_char", sym::TypeSet{m_string, m_char}, m_string);
  m_funcDecls.registerIntrinsic(
      *this, Fk::LengthString, "string_length", sym::TypeSet{m_string}, m_int);
  m_funcDecls.registerIntrinsic(
      *this, Fk::IndexString, "string_index", sym::TypeSet{m_string, m_int}, m_char);
  m_funcDecls.registerIntrinsic(
      *this, Fk::SliceString, "string_slice", sym::TypeSet{m_string, m_int, m_int}, m_string);

  // Register conversion intrinsics.
  m_funcDecls.registerIntrinsic(*this, Fk::ConvIntLong, "int_to_long", sym::TypeSet{m_int}, m_long);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ConvIntFloat, "int_to_float", sym::TypeSet{m_int}, m_float);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ConvFloatInt, "float_to_int", sym::TypeSet{m_float}, m_int);
  m_funcDecls.registerIntrinsic(*this, Fk::ConvLongInt, "long_to_int", sym::TypeSet{m_long}, m_int);
  m_funcDecls.registerIntrinsic(*this, Fk::ConvIntChar, "int_to_char", sym::TypeSet{m_int}, m_char);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ConvLongChar, "long_to_char", sym::TypeSet{m_long}, m_char);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ConvFloatChar, "float_to_char", sym::TypeSet{m_float}, m_char);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ConvFloatLong, "float_to_long", sym::TypeSet{m_float}, m_long);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ConvLongFloat, "long_to_float", sym::TypeSet{m_long}, m_float);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ConvIntString, "int_to_string", sym::TypeSet{m_int}, m_string);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ConvLongString, "long_to_string", sym::TypeSet{m_long}, m_string);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ConvFloatString, "float_to_string", sym::TypeSet{m_float}, m_string);
  m_funcDecls.registerIntrinsic(
      *this, Fk::ConvCharString, "char_to_string", sym::TypeSet{m_char}, m_string);
  m_funcDecls.registerIntrinsic(*this, Fk::NoOp, "char_as_int", sym::TypeSet{m_char}, m_int);
  m_funcDecls.registerIntrinsic(*this, Fk::NoOp, "int_as_float", sym::TypeSet{m_int}, m_float);
  m_funcDecls.registerIntrinsic(*this, Fk::NoOp, "float_as_int", sym::TypeSet{m_float}, m_int);
  m_funcDecls.registerIntrinsic(*this, Fk::NoOp, "bool_as_int", sym::TypeSet{m_bool}, m_int);

  // Source-location intrinsics.
  m_funcDecls.registerIntrinsic(
      *this, Fk::SourceLocFile, "source_loc_file", sym::TypeSet{}, m_string);
  m_funcDecls.registerIntrinsic(*this, Fk::SourceLocLine, "source_loc_line", sym::TypeSet{}, m_int);
  m_funcDecls.registerIntrinsic(
      *this, Fk::SourceLocColumn, "source_loc_column", sym::TypeSet{}, m_int);

  // Register build-in actions.
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionEndiannessNative, "platform_endianness_native", sym::TypeSet{}, m_int);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionPlatformErrorCode, "platform_error_code", sym::TypeSet{}, m_int);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionStreamCheckValid, "stream_checkvalid", sym::TypeSet{m_sysStream}, m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionStreamReadString,
      "stream_read_string",
      sym::TypeSet{m_sysStream, m_int},
      m_string);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionStreamWriteString,
      "stream_write_string",
      sym::TypeSet{m_sysStream, m_string},
      m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionStreamSetOptions,
      "stream_setoptions",
      sym::TypeSet{m_sysStream, m_int},
      m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionStreamUnsetOptions,
      "stream_unsetoptions",
      sym::TypeSet{m_sysStream, m_int},
      m_bool);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionProcessStart, "process_start", sym::TypeSet{m_string}, m_sysProcess);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionProcessBlock, "process_block", sym::TypeSet{m_sysProcess}, m_int);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionProcessOpenStream,
      "process_openstream",
      sym::TypeSet{m_sysProcess, m_int},
      m_sysStream);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionProcessGetId, "process_getid", sym::TypeSet{m_sysProcess}, m_long);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionProcessSendSignal,
      "process_sendsignal",
      sym::TypeSet{m_sysProcess, m_int},
      m_bool);

  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionFileOpenStream,
      "file_openstream",
      sym::TypeSet{m_string, m_int},
      m_sysStream);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionFileType, "file_type", sym::TypeSet{m_string}, m_int);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionFileCreateDir, "file_create_directory", sym::TypeSet{m_string}, m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionFileRemove, "file_remove", sym::TypeSet{m_string}, m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionFileRemoveDir, "file_remove_directory", sym::TypeSet{m_string}, m_bool);

  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionTcpOpenCon,
      "tcp_connection_open",
      sym::TypeSet{m_string, m_int, m_int},
      m_sysStream);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionTcpStartServer,
      "tcp_server_start",
      sym::TypeSet{m_int, m_int, m_int},
      m_sysStream);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionTcpAcceptCon, "tcp_server_accept", sym::TypeSet{m_sysStream}, m_sysStream);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionTcpShutdown, "tcp_shutdown", sym::TypeSet{m_sysStream}, m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionIpLookupAddress,
      "ip_lookupaddress",
      sym::TypeSet{m_string, m_int},
      m_string);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionConsoleOpenStream, "console_openstream", sym::TypeSet{m_int}, m_sysStream);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionIsTerm, "is_term", sym::TypeSet{m_sysStream}, m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionTermSetOptions, "term_setoptions", sym::TypeSet{m_sysStream, m_int}, m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this,
      Fk::ActionTermUnsetOptions,
      "term_unsetoptions",
      sym::TypeSet{m_sysStream, m_int},
      m_bool);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionTermGetWidth, "term_getwidth", sym::TypeSet{m_sysStream}, m_int);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionTermGetHeight, "term_getheight", sym::TypeSet{m_sysStream}, m_int);

  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionEnvGetArg, "env_argument", sym::TypeSet{m_int}, m_string);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionEnvGetArgCount, "env_argument_count", sym::TypeSet{}, m_int);
  m_funcDecls.registerIntrinsicAction(
      *this, Fk::ActionEnvHasVar, "env_variable_exists", sym::TypeSet{m_string}, m_bool);
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
      *this, Fk::ActionClockTimezoneOffset, "clock_timezoneoffset", sym::TypeSet{}, m_int);

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

auto Program::areStructsEquivalent(sym::TypeId a, sym::TypeId b) const -> bool {
  const auto& aDecl = getTypeDecl(a);
  const auto& bDecl = getTypeDecl(b);
  if (aDecl.getKind() != sym::TypeKind::Struct || bDecl.getKind() != sym::TypeKind::Struct) {
    return false;
  }
  const auto& aDef = std::get<sym::StructDef>(getTypeDef(a));
  const auto& bDef = std::get<sym::StructDef>(getTypeDef(b));
  if (aDef.getFields().getCount() != bDef.getFields().getCount()) {
    return false;
  }
  for (auto i = 0u; i != aDef.getFields().getCount(); ++i) {
    if (aDef.getFields()[i].getType() != bDef.getFields()[i].getType()) {
      return false;
    }
  }
  return true;
}

auto Program::areUnionsEquivalent(sym::TypeId a, sym::TypeId b) const -> bool {
  const auto& aDecl = getTypeDecl(a);
  const auto& bDecl = getTypeDecl(b);
  if (aDecl.getKind() != sym::TypeKind::Union || bDecl.getKind() != sym::TypeKind::Union) {
    return false;
  }
  const auto& aDef = std::get<sym::UnionDef>(getTypeDef(a));
  const auto& bDef = std::get<sym::UnionDef>(getTypeDef(b));
  if (aDef.getTypes().size() != bDef.getTypes().size()) {
    return false;
  }
  for (auto i = 0u; i != aDef.getTypes().size(); ++i) {
    if (aDef.getTypes()[i] != bDef.getTypes()[i]) {
      return false;
    }
  }
  return true;
}

auto Program::areUsertypesEquivalent(sym::TypeId a, sym::TypeId b) const -> bool {
  return areStructsEquivalent(a, b) || areUnionsEquivalent(a, b);
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

auto Program::declareStaticInt(std::string name) -> sym::TypeId {
  return m_typeDecls.registerType(sym::TypeKind::StaticInt, std::move(name));
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

auto Program::declareImplicitConv(sym::TypeId input, sym::TypeId output) -> sym::FuncId {
  return m_funcDecls.registerImplicitConv(*this, sym::FuncKind::User, input, output);
}

auto Program::declareFailIntrinsic(std::string name, sym::TypeId output) -> sym::FuncId {
  return m_funcDecls.registerIntrinsic(
      *this, sym::FuncKind::Fail, std::move(name), sym::TypeSet{}, output);
}

auto Program::declareUsertypeAliasIntrinsic(std::string name, sym::TypeId input, sym::TypeId output)
    -> sym::FuncId {
  if (!areUsertypesEquivalent(input, output)) {
    throw std::invalid_argument{"Input and output usertypes have to be equivelent"};
  }
  return m_funcDecls.registerIntrinsic(
      *this, sym::FuncKind::NoOp, std::move(name), sym::TypeSet{input}, output);
}

auto Program::defineStruct(sym::TypeId id, sym::FieldDeclTable fields) -> void {
  auto fieldTypes = std::vector<sym::TypeId>{};
  for (const auto& field : fields) {
    fieldTypes.push_back(field.getType());
  }

  // Register constructor function.
  const auto& name = m_typeDecls[id].getName();
  m_funcDecls.registerFunc(*this, sym::FuncKind::MakeStruct, name, sym::TypeSet{fieldTypes}, id);

  // Register equality intrinsic.
  m_funcDecls.registerIntrinsic(
      *this, sym::FuncKind::CheckEqUserType, "usertype_eq_usertype", sym::TypeSet{id, id}, m_bool);

  // Register struct definition.
  m_typeDefs.registerStruct(m_typeDecls, id, std::move(fields));
}

auto Program::defineUnion(sym::TypeId id, std::vector<sym::TypeId> types) -> void {

  // Register constructor functions for each type.
  for (const auto& type : types) {
    m_funcDecls.registerImplicitConv(*this, sym::FuncKind::MakeUnion, type, id);
  }

  // Register equality intrinsic.
  m_funcDecls.registerIntrinsic(
      *this, sym::FuncKind::CheckEqUserType, "usertype_eq_usertype", sym::TypeSet{id, id}, m_bool);

  // Register union definition.
  m_typeDefs.registerUnion(m_typeDecls, id, std::move(types));
}

auto Program::defineEnum(sym::TypeId id, std::vector<sym::EnumDef::Pair> entries) -> void {
  using fk = sym::FuncKind;

  const auto& name = m_typeDecls[id].getName();

  // Register explicit conversion from int.
  m_funcDecls.registerFunc(*this, fk::NoOp, name, sym::TypeSet{m_int}, id);

  // Register implicit conversion to int.
  m_funcDecls.registerImplicitConv(*this, fk::NoOp, id, m_int);

  // Register ordering operators (<, >).
  m_funcDecls.registerFunc(
      *this, fk::CheckLeInt, getFuncName(Operator::Le), sym::TypeSet{id, id}, m_bool);
  m_funcDecls.registerFunc(
      *this, fk::CheckGtInt, getFuncName(Operator::Gt), sym::TypeSet{id, id}, m_bool);

  // Register bitwise & and | operators.
  m_funcDecls.registerFunc(*this, fk::OrInt, getFuncName(Operator::Pipe), sym::TypeSet{id, id}, id);
  m_funcDecls.registerFunc(*this, fk::AndInt, getFuncName(Operator::Amp), sym::TypeSet{id, id}, id);

  // Register equality intrinsic.
  m_funcDecls.registerIntrinsic(
      *this, fk::CheckEqInt, "usertype_eq_usertype", sym::TypeSet{id, id}, m_bool);

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

} // namespace prog
