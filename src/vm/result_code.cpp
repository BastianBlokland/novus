#include "vm/result_code.hpp"

namespace vm {

auto operator<<(std::ostream& out, const ResultCode& rhs) noexcept -> std::ostream& {
  switch (rhs) {
  case ResultCode::Ok:
    out << "ok";
    break;
  case ResultCode::InvalidAssembly:
    out << "invalid-assembly";
    break;

  case ResultCode::CallStackOverflow:
    out << "call-stack-overflow";
    break;
  case ResultCode::ConstStackOverflow:
    out << "const-stack-overflow";
    break;
  case ResultCode::EvalStackOverflow:
    out << "eval-stack-overflow";
    break;
  case ResultCode::EvalStackNotEmpty:
    out << "eval-stack-not-empty";
    break;

  case ResultCode::DivByZero:
    out << "div-by-zero";
    break;
  case ResultCode::AssertFailed:
    out << "assert-failed";
    break;
  }
  return out;
}

} // namespace vm
