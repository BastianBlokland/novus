#include "novasm/pcall_code.hpp"

namespace novasm {

auto operator<<(std::ostream& out, const PCallCode& rhs) noexcept -> std::ostream& {
  switch (rhs) {
  case PCallCode::ConWriteChar:
    out << "con-write-char";
    break;
  case PCallCode::ConWriteString:
    out << "con-write-string";
    break;
  case PCallCode::ConWriteStringLine:
    out << "con-write-string-line";
    break;

  case PCallCode::ConReadChar:
    out << "con-read-char";
    break;
  case PCallCode::ConReadStringLine:
    out << "con-read-string-line";
    break;

  case PCallCode::StreamOpenFile:
    out << "stream-open-file";
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
  case PCallCode::StreamWriteChar:
    out << "stream-write-char";
    break;
  case PCallCode::StreamFlush:
    out << "stream-flush";
    break;
  case PCallCode::FileRemove:
    out << "file-remove";
    break;

  case PCallCode::GetEnvArg:
    out << "get-env-arg";
    break;
  case PCallCode::GetEnvArgCount:
    out << "get-env-arg-count";
    break;
  case PCallCode::GetEnvVar:
    out << "get-env-var";
    break;

  case PCallCode::ClockMicroSinceEpoch:
    out << "clock-micro-since-epoch";
    break;
  case PCallCode::ClockNanoSteady:
    out << "clock-nano-steady";
    break;

  case PCallCode::SleepNano:
    out << "sleep-nano";
    break;
  case PCallCode::Assert:
    out << "assert";
    break;
  }
  return out;
}

} // namespace novasm
