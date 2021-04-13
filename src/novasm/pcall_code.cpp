#include "novasm/pcall_code.hpp"

namespace novasm {

auto operator<<(std::ostream& out, const PCallCode& rhs) noexcept -> std::ostream& {
  switch (rhs) {
  case PCallCode::EndiannessNative:
    out << "endianness-native";
    break;
  case PCallCode::PlatformErrorCode:
    out << "platform-error-code";
    break;

  case PCallCode::StreamCheckValid:
    out << "stream-check-valid";
    break;
  case PCallCode::StreamReadString:
    out << "stream-read-string";
    break;
  case PCallCode::StreamWriteString:
    out << "stream-write-string";
    break;
  case PCallCode::StreamSetOptions:
    out << "stream-set-options";
    break;
  case PCallCode::StreamUnsetOptions:
    out << "stream-unset-options";
    break;

  case PCallCode::ProcessStart:
    out << "process-start";
    break;
  case PCallCode::ProcessBlock:
    out << "process-block";
    break;
  case PCallCode::ProcessOpenStream:
    out << "process-open-stream";
    break;
  case PCallCode::ProcessGetId:
    out << "process-get-id";
    break;
  case PCallCode::ProcessSendSignal:
    out << "process-send-signal";
    break;

  case PCallCode::FileOpenStream:
    out << "file-open-stream";
    break;
  case PCallCode::FileType:
    out << "file-type";
    break;
  case PCallCode::FileCreateDir:
    out << "file-create-dir";
    break;
  case PCallCode::FileRemove:
    out << "file-remove";
    break;
  case PCallCode::FileRemoveDir:
    out << "file-remove-dir";
    break;

  case PCallCode::TcpOpenCon:
    out << "tcp-open-con";
    break;
  case PCallCode::TcpStartServer:
    out << "tcp-start-server";
    break;
  case PCallCode::TcpAcceptCon:
    out << "tcp-accept-con";
    break;
  case PCallCode::TcpShutdown:
    out << "tcp-shutdown";
    break;
  case PCallCode::IpLookupAddress:
    out << "ip-lookup-address";
    break;

  case PCallCode::ConsoleOpenStream:
    out << "console-open-stream";
    break;

  case PCallCode::IsTerm:
    out << "is-term";
    break;
  case PCallCode::TermSetOptions:
    out << "term-set-options";
    break;
  case PCallCode::TermUnsetOptions:
    out << "term-unset-options";
    break;
  case PCallCode::TermGetWidth:
    out << "term-get-width";
    break;
  case PCallCode::TermGetHeight:
    out << "term-get-height";
    break;

  case PCallCode::EnvGetArg:
    out << "env-get-arg";
    break;
  case PCallCode::EnvGetArgCount:
    out << "env-get-arg-count";
    break;
  case PCallCode::EnvHasVar:
    out << "env-has-var";
    break;
  case PCallCode::EnvGetVar:
    out << "env-get-var";
    break;
  case PCallCode::InteruptIsReq:
    out << "interupt-is-req";
    break;
  case PCallCode::InteruptResetReq:
    out << "interupt-reset-req";
    break;

  case PCallCode::ClockMicroSinceEpoch:
    out << "clock-micro-since-epoch";
    break;
  case PCallCode::ClockNanoSteady:
    out << "clock-nano-steady";
    break;
  case PCallCode::ClockTimezoneOffset:
    out << "clock-timezone-offset";
    break;

  case PCallCode::VersionRt:
    out << "version-rt";
    break;
  case PCallCode::VersionCompiler:
    out << "version-compiler";
    break;

  case PCallCode::PlatformCode:
    out << "platform-code";
    break;
  case PCallCode::WorkingDirPath:
    out << "working-dir-path";
    break;
  case PCallCode::RtPath:
    out << "rt-path";
    break;
  case PCallCode::ProgramPath:
    out << "program-path";
    break;

  case PCallCode::SleepNano:
    out << "sleep-nano";
    break;
  }
  return out;
}

} // namespace novasm
