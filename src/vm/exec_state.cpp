#include "vm/exec_state.hpp"

namespace vm {

auto operator<<(std::ostream& out, const ExecState& rhs) noexcept -> std::ostream& {
  switch (rhs) {
  case ExecState::Starting:
    out << "starting";
    break;
  case ExecState::Running:
    out << "running";
    break;
  case ExecState::Success:
    out << "success";
    break;
  case ExecState::InvalidAssembly:
    out << "invalid-assembly";
    break;
  case ExecState::StackOverflow:
    out << "stack-overflow";
    break;
  case ExecState::DivByZero:
    out << "div-by-zero";
    break;
  case ExecState::AssertFailed:
    out << "assert-failed";
    break;
  }
  return out;
}

} // namespace vm
