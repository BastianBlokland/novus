#include "vm/exec_state.hpp"

namespace vm {

auto operator<<(std::ostream& out, const ExecState& rhs) noexcept -> std::ostream& {
  switch (rhs) {
  case ExecState::Aborted:
    out << "aborted";
    break;
  case ExecState::Paused:
    out << "paused";
    break;
  case ExecState::Running:
    out << "running";
    break;

  case ExecState::Success:
    out << "success";
    break;
  case ExecState::Failed:
    out << "failed";
    break;
  case ExecState::AssertFailed:
    out << "assert-failed";
    break;

  case ExecState::VmInitFailed:
    out << "vm-init-failed";
    break;
  case ExecState::InvalidAssembly:
    out << "invalid-assembly";
    break;
  case ExecState::StackOverflow:
    out << "stack-overflow";
    break;
  case ExecState::AllocFailed:
    out << "alloc-failed";
    break;
  case ExecState::DivByZero:
    out << "div-by-zero";
    break;
  case ExecState::ForkFailed:
    out << "fork-failed";
    break;
  }
  return out;
}

} // namespace vm
