#include "vm/result_code.hpp"

namespace vm {

auto operator<<(std::ostream& out, const ResultCode& rhs) noexcept -> std::ostream& {
  switch (rhs) {
  case ResultCode::Ok:
    out << "ok";
    break;
  case ResultCode::DivByZero:
    out << "div-by-zero";
    break;
  case ResultCode::StackOverflow:
    out << "stack-overflow";
    break;
  case ResultCode::AssertFailed:
    out << "assert-failed";
    break;
  case ResultCode::InvalidAssembly:
    out << "invalid-assembly";
    break;
  }
  return out;
}

} // namespace vm
